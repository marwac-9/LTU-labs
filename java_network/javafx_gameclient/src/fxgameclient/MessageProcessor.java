/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

import java.io.IOException;
import java.util.Vector;
import javafx.application.Platform;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

/**
 *
 * @author Deadline
 */
public class MessageProcessor 
{
    private Vector<PlayerInfo> players;
    private FXGameView gView;
    
    MessageProcessor(Vector<PlayerInfo> players, FXGameView gView)
    {
        this.gView = gView;
        this.players = players;
    }
    
    public void executeMessage(String message) 
    {
        String[] commands = message.split("/");
        
        showMessage("Split commands: ");
        for (int i = 0; i < commands.length; i++) 
        {
            showMessage(commands[i]);
        }

        if ((commands[0].equals("SERVER - ")) ) 
        {
            if (commands[1].equals("joinAccepted")) 
            {
                Platform.runLater(new Runnable()
                {
                    @Override
                    public void run() 
                    {
                        PlayerInfo newPlayer = new PlayerInfo(Integer.parseInt(commands[2]),Integer.parseInt(commands[3]),Integer.parseInt(commands[4]),Integer.parseInt(commands[5]),Integer.parseInt(commands[6])); 
                        //set up player
                        showMessage("adding player");
                        if (players.size() == 0) 
                        {
                            showMessage("setting color for first player");
                            //set color before adding to the stage
                            newPlayer.setFill(Color.rgb(127,127,127,0.6));
                            gView.root.getChildren().add(newPlayer);
                        }
                        else
                        {
                            showMessage("showing player");
                            gView.root.getChildren().add(newPlayer);
                        }
                        //we add player to the list at the end because keylistener watches if player entered the game and then enables key controll
                        players.add(newPlayer);
                        System.out.println(players.size());
                    }
                });
                //startGame = true;
            }
            if (commands[1].equals("moveAccepted"))
            {
                Platform.runLater(new Runnable()
                {
                    @Override
                    public void run() 
                    {
                        //that's the player from array added to the scene 
                        for (int i = 0; i < players.size(); i++) {
                            if (players.get(i).id == Integer.parseInt(commands[2])) {
                                int indexOfPlayer = gView.root.getChildren().indexOf(players.get(i));
                                //now let's get it from scene and update positions
                                gView.root.getChildren().get(indexOfPlayer).setLayoutX(Integer.parseInt(commands[3]));
                                gView.root.getChildren().get(indexOfPlayer).setLayoutY(Integer.parseInt(commands[4]));
                            }
                        }

                    }
                });
            }
            if (commands[1].equals("remove")) 
            {
                Platform.runLater(new Runnable()
                {
                    @Override
                    public void run() 
                    {
                        for (int i = 0; i < players.size(); i++) 
                        {
                            if (players.get(i).id == Integer.parseInt(commands[2])) {
                                int indexOfPlayer = gView.root.getChildren().indexOf(players.get(i));
                                gView.root.getChildren().remove(gView.root.getChildren().get(indexOfPlayer));
                                players.remove(i);
                            }
                        }

                    }
                });

            }
        }
        if((commands[0].equals("SERVER - END")))
        {
            showMessage("placeholder for something indicating that server was shut down or client lost connection with server, for example gui pop up");
        }
    }
    private void showMessage(String m)
    {
        System.out.println(m);
    }
}
        

