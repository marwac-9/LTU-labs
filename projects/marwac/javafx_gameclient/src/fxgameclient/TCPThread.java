/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.scene.paint.Color;

/**
 *
 * @author Deadline
 */
public class TCPThread extends Thread
{
    PrintWriter output;
    BufferedReader input;
    Socket socket;
    
    boolean runThread = true;
    boolean connected = true;
    
    MessageProcessor processMessage;
    
    TCPThread(Socket socket, MessageProcessor processMessage)
    {
        setupStreams(socket);
        this.processMessage = processMessage;        
    }
            
    @Override
    public void run() {
        String message = " ";
        showMessage("\nInside TPC thread");
        try{
            do{

                try {
                    message = input.readLine();
                } 
                catch(java.net.SocketException socketExcept)
                {
                    showMessage("well server aborted connection");
                    message="SERVER - END";
                    runThread=false;
                }
                catch (IOException ex) {
                    Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
                }
                if(message!=null)
                {
                    showMessage("\n" + message);
                    processMessage.executeMessage(message);
                }
            }
            while(runThread);
        }
        finally
        {
            try {
                input.close();
                output.close();
            } 
            catch (IOException ex) {
                Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public void sendMessage(String message) 
    {
        if (connected) 
        {
            output.println(message);
            output.flush();
            showMessage(message);
        }
    }
    
    private void showMessage(String s)
    {
        System.out.println(s);
    }
    
    private void setupStreams(Socket socket) 
    {
        try 
        {
            //get output link to server
            output = new PrintWriter(socket.getOutputStream(),true);
            output.flush();
       
            //get input link to server
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } 
        catch (IOException ex) 
        {
            Logger.getLogger(FXController.class.getName()).log(Level.SEVERE, null, ex);
        }
        showMessage("\nClient streams are now good to go! \n");
    }
}
