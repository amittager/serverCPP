#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <mutex>

std::unordered_map<std::string, std::unordered_set<std::string>> userWatchHistory; // User to watched video IDs
std::unordered_map<std::string, int> videoPopularity; // Video ID to views count
std::mutex mtx; // Mutex for thread safety

// Function to update user watch history and video popularity
void updateWatchHistory(const std::string& userId, const std::string& videoId) {
    std::lock_guard<std::mutex> lock(mtx);
    userWatchHistory[userId].insert(videoId);
    videoPopularity[videoId]++;
    std::cout << "Updated watch history for user: " << userId << ", video: " << videoId << std::endl;
}

// Function to generate recommendations for a given video ID
std::vector<std::string> generateRecommendations(const std::string& videoId) {
    std::lock_guard<std::mutex> lock(mtx);
    std::unordered_map<std::string, int> recommendationScores;

    for (const auto& [user, videos] : userWatchHistory) {
        if (videos.find(videoId) != videos.end()) {
            for (const auto& watchedVideo : videos) {
                if (watchedVideo != videoId) {
                    recommendationScores[watchedVideo] += videoPopularity[watchedVideo];
                }
            }
        }
    }

    // Sort recommendations by popularity score
    std::vector<std::pair<std::string, int>> scoredRecommendations(
        recommendationScores.begin(), recommendationScores.end());
    std::sort(scoredRecommendations.begin(), scoredRecommendations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Extract top 6-10 recommended video IDs
    std::vector<std::string> recommendations;
    for (size_t i = 0; i < std::min(scoredRecommendations.size(), size_t(10)); ++i) {
        recommendations.push_back(scoredRecommendations[i].first);
    }

    std::cout << "Generated recommendations for video: " << videoId << " - "
              << recommendations.size() << " recommendations" << std::endl;
    return recommendations;
}

// Handle client requests for watching and recommending videos
void handleClient(int client_sock) {
    char buffer[4096];
    int read_bytes;

    while ((read_bytes = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[read_bytes] = '\0';
        std::string request(buffer);
        std::string response;

        if (request.find("WATCH") == 0) {
            // Parse WATCH command: "WATCH <userId> <videoId>"
            std::istringstream iss(request);
            std::string command, userId, videoId;
            iss >> command >> userId >> videoId;

            if (!userId.empty() && !videoId.empty()) {
                updateWatchHistory(userId, videoId);
                response = "WATCH_UPDATED";

                // Generate recommendations after updating the watch history
                auto recommendations = generateRecommendations(videoId);
                std::ostringstream oss;
                oss << "[";

                // Format recommendations as a JSON array
                for (const auto& vid : recommendations) {
                    oss << "\"" << vid << "\","; // Add video IDs
                }
                if (!recommendations.empty()) {
                    response += ", Recommendations: [";
                    response += oss.str();
                    response.back() = ']';  // Replace trailing comma with closing bracket
                } else {
                    response += ", Recommendations: []";
                }

            } else {
                response = "ERROR: Invalid WATCH command format";
            }
        } else if (request.find("RECOMMEND_FOR_VIDEO") == 0) {
            // Parse RECOMMEND_FOR_VIDEO command: "RECOMMEND_FOR_VIDEO <videoId>"
            std::istringstream iss(request);
            std::string command, videoId;
            iss >> command >> videoId;

            if (!videoId.empty()) {
                auto recommendations = generateRecommendations(videoId);
                std::ostringstream oss;
                oss << "[";

                // Format recommendations as a JSON array
                for (const auto& vid : recommendations) {
                    oss << "\"" << vid << "\","; // Add video IDs
                }
                if (!recommendations.empty()) {
                    response = oss.str();
                    response.back() = ']';  // Replace trailing comma with closing bracket
                } else {
                    response = "[]";
                }
            } else {
                response = "ERROR: Invalid RECOMMEND_FOR_VIDEO command format";
            }
        } else {
            response = "ERROR: Unrecognized command";
        }

        // Send response back to the client
        send(client_sock, response.c_str(), response.size(), 0);
    }

    close(client_sock);
}

int main() {
    const int server_port = 5555; // Port number for the server
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    if (bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        perror("Error listening on socket");
        close(server_sock);
        return 1;
    }

    std::cout << "C++ server listening on port " << server_port << std::endl;

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_len);
        if (client_sock < 0) {
            perror("Error accepting client");
            continue;
        }

        // Handle each client in a separate thread
        std::thread clientThread(handleClient, client_sock);
        clientThread.detach();
    }

    close(server_sock);
    return 0;
}
