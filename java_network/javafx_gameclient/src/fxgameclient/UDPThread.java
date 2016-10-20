/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Socket;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Deadline
 */
public class UDPThread implements Runnable{
    
    PrintWriter output;
    BufferedReader input;
    DatagramPacket Packet;
    TCPThread TCPThread;
    Vector<PlayerInfo> players;
    DatagramSocket udpSocket;
    MessageProcessor msgProcess;
    Socket socket;
    
    UDPThread(TCPThread TCPThread, Vector<PlayerInfo> players, Socket socket, DatagramSocket udpSocket, MessageProcessor msgProcess)
    {
        this.TCPThread = TCPThread;
        this.players = players;
        this.udpSocket = udpSocket;
        this.msgProcess = msgProcess;
        this.socket = socket;
    }
    
    
    @Override
    public void run() {
        try
        {
            showMessage("Inside UDP thread");
            whileChatting();
        }
        finally
        {
            try{
                showMessage("closing client");
                showMessage("client leaving game");
                if (players.size() > 0) {
                    TCPThread.sendMessage("CLIENT - /leave/"+players.get(0).id);
                }
                TCPThread.sendMessage("CLIENT - END");
                showMessage("client closing socket and streams");
                closeSocket();                    
                output.close();
                input.close();
            }
            catch(IOException ioException)
            {ioException.printStackTrace();}

        }
    }


    //while chatting with server
    private void whileChatting() 
    {
        String message = null;

        showMessage("\nInside whileChattng");
        do{
            try {
                showMessage("\nDo while chatting");
                byte[] buffer = new byte[100];
                Packet = new DatagramPacket(buffer, buffer.length);
                try {
                    udpSocket.receive(Packet);
                } catch (IOException ex) {
                    Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
                }
                message = new String(Packet.getData(),"ASCII").trim();
                showMessage("Message from UDP: " + message);
                if(message!=null)
                {
                    msgProcess.executeMessage(message);
                }
            } catch (UnsupportedEncodingException ex) {
                Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        //run this cunnection until this message is sent
        //while(!message.equals("SERVER - END"));
        while(true);
    }

    private boolean PlayerAlreadyExist(int playerid)
    {
        for (int i = 0; i < players.size(); i++) {
            if (players.get(i).id == playerid) {
                return true;
            }
        }
        return false;
    }

    private void closeSocket() throws IOException
    {
        showMessage("\nClosing socket down and STAGE");
        try
        {
            socket.close();
//            Platform.runLater(new Runnable()
//            {
//                @Override
//                public void run() 
//                {
//                    Stage sc = (Stage) gView.refreshBtn.getScene().getWindow();
//                    sc.close();
//                }
//            });
        }
        catch(IOException ioException)
        {ioException.printStackTrace(); }
    }
    
    private void showMessage(String m)
    {
        System.out.println(m);
    }
    
}
