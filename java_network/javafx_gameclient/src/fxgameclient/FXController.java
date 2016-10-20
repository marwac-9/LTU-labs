/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

import java.io.EOFException;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.animation.AnimationTimer;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.stage.WindowEvent;

/**
 *
 * @author Deadline
 */

public class FXController implements Runnable {

    private FXGameView gView;
    Vector<PlayerInfo> players = new Vector<PlayerInfo>();
    
    MulticastSocket mcSocket = null;
    Socket socket = null;
    DatagramSocket udpSocket = null;
    DatagramPacket mcPacket;
    InetAddress msIA;
    
    boolean requestToSetUpConnection = false;
    boolean connected = false;
    int running = 1; // used for alternate way to close client with ESC button
    
    TCPThread TCPThread;
    AnimationTimer keyListener;
    MessageProcessor msgProcess;
    
            
    FXController(FXGameView view)
    {
        this.gView = view;
        this.gView.addMovementOnKeyPress(new MovementOnKeyPress());
        this.gView.addMovementOnKeyRelease(new MovementOnKeyRelease());
        this.gView.addRefreshOnButonPress(new RefreshButtonAction());
        this.gView.addActionOnRowDoubleClick(new ActionOnRowDoubleClick());
        this.gView.addEventOnClose(new EventOnClose());
        this.gView.show();
        this.msgProcess = new MessageProcessor(players,gView);
        this.keyListener = new AnimationTimer() 
        {
            @Override
            public void handle(long timestamp)
            {
                if(requestToSetUpConnection)
                {
                    setupConnectionToServer();
                    requestToSetUpConnection = false;
                }
                if (gView.isClose()) //if you press close button while connected to server
                {
                    showMessage("closing client");
                    if (!mcSocket.isClosed()) // multicast is closed att connection so it shouldn't be the case
                    {
                        showMessage("leaving multicast group"); 
                        try {
                            mcSocket.leaveGroup(msIA);
                            mcSocket.close();
                        } catch (IOException ex) {
                            Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
                        }
                    }
                    if (connected) {
                        showMessage("disconnecting from server");
                        TCPThread.sendMessage("CLIENT - /leave/"+players.get(0).id);
                        TCPThread.sendMessage("CLIENT - END");

                    }
                }
                if (players.size() > 0) 
                {
                    if (running == 0) // if you press ESC button
                    {
                        running++;
                        showMessage("closing client");
                        if (!mcSocket.isClosed()) 
                        {
                            showMessage("leaving multicast group");
                            try {
                                mcSocket.leaveGroup(msIA);
                                mcSocket.close();
                            } catch (IOException ex) {
                                Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
                            }
                        }
                        if (connected) {
                            showMessage("disconnecting from server");
                            TCPThread.sendMessage("CLIENT - /leave/"+players.get(0).id);
                            TCPThread.sendMessage("CLIENT - END");
                        }                                
                    }
                    if (gView.isDown()) 
                    {
                        TCPThread.sendMessage("CLIENT - /move/down/"+players.get(0).id);                    
                    }
                    if (gView.isUp())
                    {
                        TCPThread.sendMessage("CLIENT - /move/up/"+players.get(0).id);   
                    }
                    if (gView.isLeft())
                    {
                        TCPThread.sendMessage("CLIENT - /move/left/"+players.get(0).id);    
                    }
                    if (gView.isRight())
                    {
                        TCPThread.sendMessage("CLIENT - /move/right/"+players.get(0).id);   
                    }
                }
            }
        };
    }


    @Override
    public void run() {
       
        
        try 
        {
            showMessage("Attempting multicast connection...");
            mcSocket = new MulticastSocket(1900);
            msIA = InetAddress.getByName("224.2.2.4");
            mcSocket.joinGroup(msIA);
            showMessage("Joined Multicast group at: " + msIA.getHostName() + " at port: " + mcSocket.getLocalPort() + "\n");
            if (mcSocket.isBound()) 
            {
                Platform.runLater(new Runnable()
                {
                    @Override
                    public void run() 
                    {
                        gView.showTable();
                    }
                });

                whileMCChatting();
            }
        } 
        
        catch(EOFException eofException)
        {
            showMessage("Client terminated connection");
        }
        catch(IOException ioException){
           ioException.printStackTrace();
        }
        finally
        {
            showMessage("closing multicast thread");
            if (!mcSocket.isClosed()) 
            {
                showMessage("leaving multicast group");
                try {
                    mcSocket.leaveGroup(msIA);
                    mcSocket.close();
                } catch (IOException ex) {
                    Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
    }
    
    //while chatting with multicast group
    private void whileMCChatting() throws IOException
    {
        String message = null;

        showMessage("\nInside whileChattng");
        while(!mcSocket.isClosed())
        {
            byte[] buffer = new byte[100];
            mcPacket = new DatagramPacket(buffer, buffer.length);
            try{
            mcSocket.receive(mcPacket);}
            catch(java.net.SocketException excpetion)
            { 
                //showMessage("multicast socket already closed");
            }
            message = new String(mcPacket.getData(),"ASCII").trim();   
            if (!mcSocket.isClosed()) {
                showMessage("Message of multicast: " + message);
            }
            if(message!=null)
            {
                executeMCMessage(message);
            }
        }        
    }
    
    private void executeMCMessage(String message) 
    {
        String[] spaceCommands = message.split(" ");
        if (spaceCommands.length == 6) 
        {
            if (spaceCommands[1].equals("REPLY")) 
            {
                Platform.runLater(new Runnable()
                {
                    @Override
                    public void run() 
                    {
                        if (gView.ServerList.isEmpty()) 
                        {
                            int newId = gView.ServerList.size();
                            //NOW EXTRACT DATA
                            ServerInfo newServer = new ServerInfo(newId, spaceCommands[3], spaceCommands[4], Integer.parseInt(spaceCommands[5]));
                            //IT WILL BE ADDED ONE SERVER AT TIME ONE ROW ATT TIME
                            gView.ServerList.add(newServer);
                            System.out.println(("Number of servers: " + gView.ServerList.size()));
                        }
                        else
                        {
                            for (int i = 0; i < gView.ServerList.size(); i++) 
                            {
                                if (!gView.ServerList.get(i).getIP().equals(spaceCommands[4])) 
                                {
                                    int newId = gView.ServerList.size();
                                    //NOW EXTRACT DATA
                                    ServerInfo newServer = new ServerInfo(newId, spaceCommands[3], spaceCommands[4], Integer.parseInt(spaceCommands[5]));
                                    //IT WILL BE ADDED ONE SERVER AT TIME ONE ROW ATT TIME
                                    gView.ServerList.add(newServer);
                                    System.out.println(("Number of servers: " + gView.ServerList.size()));
                                }
                            }
                        }
                    }
                });
            }
        }
    }
       
    private void sendMCMessage(String message) 
    {
        byte[] data = message.getBytes();
        mcPacket = new DatagramPacket(data, data.length, msIA, 1900);
        try {
            mcSocket.send(mcPacket);
        } catch (IOException ex) {
            Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    class MovementOnKeyPress implements EventHandler<KeyEvent>
    {

        @Override public void handle(KeyEvent event) 
        {
            switch (event.getCode()) 
            {
                case ESCAPE:
                    System.out.println("pressed on escape");
                    running = 0;
                    break;
                case UP:
                    gView.up = true;
                break;
                case RIGHT:  
                    gView.right = true;
                break;
                case DOWN: 
                    gView.down = true;
                break;
                case LEFT: 
                    gView.left = true;
                break;
            }
        }
    }
    
    class MovementOnKeyRelease implements EventHandler<KeyEvent>
    {

        @Override public void handle(KeyEvent event) 
        {
            switch (event.getCode()) 
            {
                case UP:
                    gView.up = false;
                break;
                case RIGHT:  
                    gView.right = false; 
                break;
                case DOWN: 
                    gView.down = false;
                break;
                case LEFT: 
                    gView.left = false; 
                break;
            }
        }
    }
    
    class RefreshButtonAction implements EventHandler<ActionEvent>
    {

        @Override
        public void handle(ActionEvent event) {
            if (gView.ServerList.size() > 0) {
                gView.ServerList.removeAll(gView.ServerList);
            }
            sendMCMessage("SERVICE QUERY JavaGameServer");
            running = 1;
        }
        
    }
    
    class ActionOnRowDoubleClick implements EventHandler<MouseEvent>
    {

        @Override
        public void handle(MouseEvent event) {
            if (event.getClickCount() > 1) {
                running = 1;
                if (gView.ServerTable.getSelectionModel().getSelectedIndex() >= 0) {
                    
                    String hostname = gView.ServerTable.getSelectionModel().getSelectedItem().getIP();
                    int port = gView.ServerTable.getSelectionModel().getSelectedItem().getPort();
                    System.out.println(hostname + " " + port);
                    try {
                    InetAddress serverIA = InetAddress.getByName(hostname);
                    connectToServer(serverIA,port);
                    } catch (UnknownHostException ex) {
                        Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
            }
            
        }  
    }
    
    class EventOnClose implements EventHandler<WindowEvent>
    {
        @Override
        public void handle(WindowEvent event) {
            gView.close = true;
        }
    }
    
    
    public void connectToServer(InetAddress hostname, int port)
    {
    showMessage("Attempting server connection...");
            try 
            {
                socket = new Socket(hostname, port);
                udpSocket = new DatagramSocket(socket.getLocalPort());
                requestToSetUpConnection = true;
                connected = true;
                UDPThread messageHandling = new UDPThread(TCPThread,players,socket,udpSocket,msgProcess);   
                Thread clientListenThread = new Thread(messageHandling);
                clientListenThread.setDaemon(true);
                clientListenThread.start();  
                keyListener.start();
            } 
            catch (IOException ex) {
                System.out.println("timeout");
            }
            
    }
    
    public void setupConnectionToServer()
    {
        if (socket.isConnected()) {
            showMessage("Connected to server " + socket.getInetAddress().getHostName());
            showMessage("UDP socket is set up ");
            Platform.runLater(new Runnable() {
                @Override public void run() {
                    gView.hideTable();
                }
            });
            if (!mcSocket.isClosed()) 
            {
                showMessage("leaving multicast group");
                try {
                    mcSocket.leaveGroup(msIA);
                    mcSocket.close();
                } catch (IOException ex) {
                    Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            TCPThread = new TCPThread(socket, msgProcess);
            TCPThread.setDaemon(true);
            TCPThread.start();
            TCPThread.sendMessage("CLIENT - /join");
            showMessage("\nSent join");

        }
    }
    
    private void showMessage(String m)
    {
        System.out.println(m);
    }
    
}
