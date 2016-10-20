/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameserver;

import java.io.BufferedReader;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Writer;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;

/**
 *
 * @author Deadline
 */
public class FXGameServer implements Runnable
{
    public final static int PORT = 13;
    
    private ServerSocket server; //starts a server at specified port
    private Socket connection; //socket listens for connection and establishes one 
    private DatagramSocket udpSocket;
    private MulticastSocket msSocket;
    
    private Vector<Player> players = new Vector<Player>();
    private List<ServerThread> threads = new ArrayList<>();
    private Game gameInfo = new Game();
    
    private boolean multiCast = true;
    private InetAddress msIA = null;
    
    private class Game
    {
        int sizeW = 600;
        int sizeH = 500;
    }
    
    
    private class CharacterClass1 extends Player
    {
        private final int speed = 5;
    }
    private class CharacterClass2 extends Player
    {
        private final int speed = 2;
    }
    
    
    private void sendUDPMessageToAll(String message)
    {
        for (int i = 0; i < threads.size(); i++) {
            threads.get(i).sendUDPMessage(message);
        }
    }
    
    //sends join message to all besides the player that joined himself because he will receive different message with all players on server
    private void sendTCPMessageToAll(String message, long threadID)
    {
        for (int i = 0; i < threads.size(); i++) {
            if (threads.get(i).getId() != threadID) {
                threads.get(i).sendTCPJoinMessage(message);
            }
        }
    }
    

    @Override
    public void run() {
        try
        {  
            System.out.println("inside a try");
            server = new ServerSocket(PORT);
            connectToMulticast();
            
            //START THREAD WITH MULTICAST TO LISTEN TO REQUESTS FOR INFORMATION ABOUT SERVER AND SEND IT AT THE SAME TIME AWAIT CONNECTION SO PLAYERS CAN JOIN SERVER
            MulticastThread multicastListener = new MulticastThread();
            Thread multiCastLstnr = new Thread(multicastListener);
            multiCastLstnr.setDaemon(true);
            multiCastLstnr.start();

            while (true) 
            {
                System.out.println("inside server while");
                try 
                {   
                    waitForConnection();
                    ServerThread task = new ServerThread(connection);
                    task.setDaemon(true);
                    threads.add(task);
                    System.out.println("number of threads: " + threads.size());
                    task.start();
                    
                } 
                catch(EOFException eofException)
                {
                   showMessage("\nServer ended the connection!");
                }
            }
        } 
        catch (IOException ex) {
            System.err.println("Couldn't start server");
        }
        finally
        {
            if (msSocket.isBound()) {
                try {
                    showMessage("shutting down server");
                    showMessage("leaving multicast group");
                    msSocket.leaveGroup(msIA);
                    showMessage("closing connection and server");
                    connection.close();
                    server.close();
                } catch (IOException ex) {
                    Logger.getLogger(FXGameServer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            
        }
    }
    
    private void connectToMulticast() throws IOException
    {
        showMessage("Attempting connection...");
        msSocket = new MulticastSocket(1900);
        msIA = InetAddress.getByName("224.2.2.4");
        msSocket.joinGroup(msIA);
        showMessage("Joined Multicast group at: " + msIA.getHostName() + " at port: " + msSocket.getLocalPort() + "\n");
    }
    
    private void showMessage(String message)
    {
        System.out.println(message);
    }
    
    
    //wait for connection then display connection information
    private void waitForConnection() throws IOException
    {
        showMessage("Waiting for someone to connect\n");
        udpSocket = new DatagramSocket();
        connection = server.accept();
        showMessage("Now connected to " + connection.getInetAddress() + " from: " +connection.getInetAddress().getHostName() + "\n" + "remote address: " + connection.getRemoteSocketAddress().toString()); 
        showMessage("UDP is set up" + "\n");
    }
    
    private class MulticastThread implements Runnable
    {
        private DatagramPacket mcPacket;


        @Override
        public void run() {
            try {
                whileChatting();
            } catch (IOException ex) {
                Logger.getLogger(FXGameServer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        
        //during the chat conversation
        private void whileChatting() throws IOException
        {
            String message;
            showMessage("while chatting");            
            do
            {
                byte[] buffer = new byte[100];
                mcPacket = new DatagramPacket(buffer, buffer.length);
                msSocket.receive(mcPacket);
                message = new String(mcPacket.getData(),"ASCII").trim();    
                showMessage("Message of multicast: " + message);
                if(message!=null)
                {
                    showMessage("\n" + message);
                    executeMessage(message);
                }
            }
            while(!message.equals("CLIENT - END"));
            showMessage("ending while loop");
        }
        
        private void executeMessage(String message) 
        {
            //write command executor
            showMessage("message received: " + message + ", executing now");
            if (message.equals("SERVICE QUERY JavaGameServer")) 
            {
                sendMCMessage("SERVICE REPLY " + "JavaGameServer " + "MasterOfPuppets " + "192.168.0.100 " + "13");
            }
        }
        
        //send a message to multicast group
        private void sendMCMessage(String message)
        {
                byte[] data = message.getBytes();
                mcPacket = new DatagramPacket(data, data.length, msIA, 1900);
                try {
                    msSocket.send(mcPacket);
                } catch (IOException ex) {
                    Logger.getLogger(FXGameServer.class.getName()).log(Level.SEVERE, null, ex);
                }
                showMessage("message that was send: " + message);
        }
    }
    
    
    private class ServerThread extends Thread 
    {
        private Socket connection;
        private PrintWriter output;
        private BufferedReader input;
        private DatagramPacket Packet;
        
        ServerThread(Socket connection) 
        {
            this.connection = connection;
            try {
                setupStreams();
            } 
            catch(IOException ioException)
            {
                ioException.printStackTrace();
            }
        }
        @Override
        public void run() 
        {
            try {
                showMessage("Starting thread: " + this.getId());
                whileChatting();
                showMessage("Closing thread: " + this.getId());
            } 
            catch (IOException ioException) {
                //System.err.println(ex);
                ioException.printStackTrace();
            }
            finally 
            {
                try 
                {
                    System.out.println("closing connection"); 
                    input.close();
                    output.close();
                    connection.close();
                    System.out.println(threads.size());
                    for (int i = 0; i < threads.size(); i++) {
                        if (threads.get(i).getId() == this.getId()) {
                            for (int j = 0; j < players.size(); j++) 
                            {
                                if (players.get(j).id == this.getId()) {
                                    sendTCPMessageToAll("SERVER - /remove"+"/"+players.get(j).id,this.getId());
                                    players.remove(j);
                                }
                            }
                            threads.remove(i);
                        }
                    }
                } 
                catch (IOException e) {
                    //e.printStackTrace();
                }
            }
        }
        
        //send a message to client
        private void sendUDPMessage(String message)
        {
            byte[] data = message.getBytes();
            InetAddress server = connection.getInetAddress();
     
            Packet = new DatagramPacket(data,data.length,server, connection.getPort());
            try {
                udpSocket.send(Packet);
            } catch (IOException ex) {
                Logger.getLogger(FXGameServer.class.getName()).log(Level.SEVERE, null, ex);
            }
            showMessage("\nSERVER UDP - " + message);
        }
        
        //send a message to client
        private void sendTCPJoinMessage(String message)
        {
            output.println(message);
            output.flush();
            showMessage("\nSERVER TCP - " + message);
        }

        private void closeSocketAndStreams() 
        {
            showMessage("\n Closing connections... ");
            try
            {
                output.close();
                input.close();
                connection.close();
            }
            catch(IOException ioException)
            {
                ioException.printStackTrace();
            }
        }
        
        //get stream to send and receive data
        private void setupStreams() throws IOException
        {
            //get output link to client
            output = new PrintWriter(connection.getOutputStream(),true);
            output.flush();
            //get input link to client
            input = new BufferedReader(new InputStreamReader(connection.getInputStream()));
            showMessage("\nServer TCP streams are now set up");
        }

        //during the chat conversation
        private void whileChatting() throws IOException
        {
            String message = "You are now connected! ";
            sendUDPMessage(message);
            showMessage("while chatting");
            do
            {
                try{
                message = input.readLine();
                }
                catch(java.net.SocketException socketExcept)
                {
                    showMessage("well client aborted connection(maybe disconnected) but nothing to worry about\n because connection will be closed by server and it's thread\n player will be removed as well at finally when thread closes in a sec");
                    //i could close connection and remove thread and player from the list here but i like it to be at finally this function does not get too messy
                    message="CLIENT - END";
                }
                if(message!=null)
                {
                    showMessage("\n" + message);
                    executeMessage(message);
                }  
            }
            while(!message.equals("CLIENT - END"));
            showMessage("ending while loop, means closing thread");
        }

        private void showMessage(String message)
        {
            System.out.println(message);
        }

        private void executeMessage(String message) 
        {
            //write command executor
            String[] commands = message.split("/");
            for (int i = 0; i < commands.length; i++) {
                showMessage(commands[i]);
            }
            if ((commands[0].equals("CLIENT - ")) ) 
            {
                if (commands[1].equals("join")) 
                {
                    //also if disconnected save position and id compare if ip or mac of that user is same
                    Player newPlayer = new Player();
                    players.add(newPlayer);
                    players.lastElement().id = this.getId();
                    
                    if (players.size() > 1) {
                        sendTCPMessageToAll("SERVER - /joinAccepted"+"/"+players.lastElement().id+"/"+players.lastElement().posx+"/"+players.lastElement().posy+"/"+players.lastElement().width+"/"+players.lastElement().height+"/"+gameInfo.sizeW+"/"+gameInfo.sizeH,this.getId());
                        showMessage("message send to other players: " + "SERVER - /joinAccepted"+"/"+players.lastElement().id+"/"+players.lastElement().posx+"/"+players.lastElement().posy+"/"+players.lastElement().width+"/"+players.lastElement().height+"/"+gameInfo.sizeW+"/"+gameInfo.sizeH);
                    }
                    
                    
                    for (int i = players.size()-1; i > -1 ; i--) {
                        sendTCPJoinMessage("SERVER - /joinAccepted"+"/"+players.get(i).id+"/"+players.get(i).posx+"/"+players.get(i).posy+"/"+players.get(i).width+"/"+players.get(i).height+"/"+gameInfo.sizeW+"/"+gameInfo.sizeH);  
                    }

                    
                }
                if (commands[1].equals("move"))
                {
                    if(commands[2].equals("up"))
                    {
                        for (int i = 0; i < players.size(); i++) {
                            if (players.get(i).id == Integer.parseInt(commands[3])) {

                                if (players.get(i).posy > 0) 
                                {
                                    players.get(i).posy = players.get(i).posy - players.get(i).speed;
                                    sendUDPMessageToAll("SERVER - /moveAccepted"+"/"+players.get(i).id+"/"+players.get(i).posx+"/"+players.get(i).posy);
                                }
                            }
                        }

                    }
                    if(commands[2].equals("down"))
                    {
                        for (int i = 0; i < players.size(); i++) {
                            if (players.get(i).id == Integer.parseInt(commands[3])) {

                                if (players.get(i).posy < gameInfo.sizeH - players.get(i).height) 
                                {
                                    players.get(i).posy = players.get(i).posy + players.get(i).speed;
                                    sendUDPMessageToAll("SERVER - /moveAccepted"+"/"+players.get(i).id+"/"+players.get(i).posx+"/"+players.get(i).posy);
                                }                          
                            }
                        }
                    }    
                    if(commands[2].equals("left"))
                    {
                        for (int i = 0; i < players.size(); i++) {
                            if (players.get(i).id == Integer.parseInt(commands[3])) {

                                if (players.get(i).posx > 0) {
                                    players.get(i).posx = players.get(i).posx - players.get(i).speed; 
                                    sendUDPMessageToAll("SERVER - /moveAccepted"+"/"+players.get(i).id+"/"+players.get(i).posx+"/"+players.get(i).posy);
                                }                            
                            }
                        }
                    }    
                    if(commands[2].equals("right"))
                    {
                        for (int i = 0; i < players.size(); i++) {
                            if (players.get(i).id == Integer.parseInt(commands[3])) {

                                if (players.get(i).posx < gameInfo.sizeW - players.get(i).width) {
                                    players.get(i).posx = players.get(i).posx + players.get(i).speed;
                                    sendUDPMessageToAll("SERVER - /moveAccepted"+"/"+players.get(i).id+"/"+players.get(i).posx+"/"+players.get(i).posy);
                                }
                            }
                        }
                    }
                }
                if (commands[1].equals("leave")) 
                {
                    for (int i = 0; i < players.size(); i++) 
                    {
                        if (players.get(i).id == Integer.parseInt(commands[2])) {
                            sendTCPMessageToAll("SERVER - /remove"+"/"+players.get(i).id,this.getId());
                            players.remove(i);
                        }
                    }
                }
            }  
        }  
    }
}
