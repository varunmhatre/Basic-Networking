# Basic-Networking

Simple client-server architecture<br><br>

Open and run server.<br>
Open and run client.<br>
-Messages sent from client is recieved by server.<br>
-Server sends a quote in response.<br><br>

Multi clients chat<br>
-Open and run multi client server.<br>
-Open multiple clients (I used putty)
-Messages sent from client is recieved by server.<br>
-Messages are then sent to other clients synchronously using fd_set.<br>
