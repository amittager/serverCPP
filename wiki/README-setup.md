1.cpp server - 
open vscode via wsl and clone this repository.
first we need the ip of the wsl so in the terminal write "hostname -I" we will use it in the node.js server so save it.
in the project open the terminal and write these instructions:
first compile: "g++ -o server server.cpp"
then execute: "./server"

2. node.js server + mongoDB -
1.clone the "faketube-sever" repository.
2.in the project file there is folder named "mongo database" which inside have 4 json files, open mongodb compass and press add new connecotion, put this in the url for connection "mongodb://localhost:27017" and press connect.
3.in the connection you just created press on the + sign to create new database, in the database name type "Faketube" and in the collection name type "users".
4.on the same window create 3 more collections under the Faketube database with the names : "comments", "reactions" and "videos".
5.now in every collection you made (should be 4 total) press on "ADD DATA" and than on "import JSON or CSV file" and choose the one that fit from the mongo database folder in the project and press import, for example for "users" collection choose the "Faketube.users.json" file.
6.after made it to the 4 collections under the Faketube database, the code for this part is on a branch we should move to, in the vs code press on the main in the bottom corner of the screen and than choose the other branch called "part-4-tcp".
7. go to server.js file and in every place there the line "const cppServerHost = '172.25.48.170'"
8. replace this number with the your wsl ip you get from the c++ server instructions it should be 2 changes in this file, now go to the videoController file and make the same for this line there (one change here).
open the terminal in the project and write "npm i".
7.still in the terminal write "nodemon .\server.js", now the server should be running.


3 for android client -
1 - clone the project
2 - for this part we use a specific branch so in android studio click on git and main branch, than on remote make sure you have no commits if there is press rollback, than choose the branch called "client-part4-(working-with-cpp-server) and than click on checkout.
3 - check the local.properties file and put there the location of your SDK in your computer.
4 - in the android studio, go to file and than press on "Sync project with gradle files"
5 - in android studio again do:
   -press on build project/make project (hammer icon)
   

4 for web client -
we described above how to init the mongo database and the server so the server will work with it, make sure you do that also. 
the server should be updated so you dont really need to do it in case you want to try rebuild the client here are the instructions:
1.clone the repository 
2.in the vs code press on the main in the bottom corner of the screen and than choose the other branch called "part-4-tcp".
2.in the project open terminal and write "npm i" to download all the Dependencies. 
3.still in the terminal write "npm run build", after that a folder named build will appear.
4.copy all the things under the new build folder and paste them under the public folder in the server project, make sure to delete all the duplicates files/folders and just the new ones that you copied will be there.
5. run the node.js server code described above.
