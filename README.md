HW #6: Distributed Wordle
Made by: Kefir Sade-Bar (ID: 921214234), Shrey Gupta (ID: 921287685), Brandon David Zarate Estrada (ID: 915430532), Surabhi Jain (ID: 919747807)

The purpose of this program is to communicate with a wordle server to solve wordles. Each guess will be outputted to the terminal and the knowledge gained will be shown through green, yellow, and grey letters. Once the word has been guessed, the json value for the game will be outputted.


Hosting the wordle server yourself (reccomended):

1. Make sure httpclient is using http://127.0.0.1:8384 on line 45 of hw6client.cpp.
2. To run the code, open up the terminal, then switch to the wsl console (type wsl in the terminal) and then **run the `make` command**.
3. Run `./hw6server` to start the wordle server.
4. Open up a split terminal, run wsl and then `./hw6client` to run the client (wordle guesser).


Each game will be logged as both a .json in the games folder, and added to the hw5server.log in the config folder.

---------------------------------------------------------------------------------------------------

Using the already hosted wordle server (server will probably be closed):

1. Make sure httpclient is using https://2ada-73-66-168-157.ngrok.io on line 45 of hw6client.cpp.
2. To run the code, open up the terminal, then switch to the wsl console (type wsl in the terminal) and then **run the `make` command**.
3. Run `./hw6client` to run the client (wordle guesser).







