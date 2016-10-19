/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameserver;

import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.effect.Bloom;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

/**
 *
 * @author Deadline
 */
public class FXServerMain extends Application{
    public static void main(String[] args)
    {
        
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
       
        Button btn = new Button();
        btn.setText("Get IP");
        btn.setLayoutX(50);
        btn.setLayoutY(80);
        Label label1 = new Label("URL:");
        label1.setLayoutX(20);
        label1.setLayoutY(53);
        TextField textField = new TextField ();
        textField.setLayoutX(50);
        textField.setLayoutY(50);
        TextField IPText = new TextField();
        IPText.setLayoutX(50);
        IPText.setLayoutY(130);
        
        btn.setOnAction(new EventHandler<ActionEvent>() {
            
            @Override
            public void handle(ActionEvent event) {
                try 
                {
                    InetAddress[] test = InetAddress.getAllByName(textField.getText());
                    Inet6Address ipv6 = getIPv6Addresses(test);
                    if (ipv6 == null) //of there is no ipv6 use first ipv4
                    {
                        IPText.setText(test[0].getHostAddress());
                    }
                    else
                    {
                        IPText.setText(ipv6.getHostAddress());
                    }
                } 
                catch (UnknownHostException ex) 
                {
                    ex.printStackTrace();
                }
            }
        });
        
        textField.setOnKeyPressed(new EventHandler<KeyEvent>()
        {

            @Override
            public void handle(KeyEvent event) {
                if (event.getCode().equals(KeyCode.ENTER))
                {
                    try {
                        InetAddress[] test = InetAddress.getAllByName(textField.getText());
                        Inet6Address ipv6 = getIPv6Addresses(test);
                        if (ipv6 == null) //of there is no ipv6 use first ipv4
                        {
                            IPText.setText(test[0].getHostAddress());
                        }
                        else
                        {
                            IPText.setText(ipv6.getHostAddress());
                        }
                    } catch (UnknownHostException ex) {
                        ex.printStackTrace();
                    } 

                }
            }
        });

        
        Pane root = new Pane();
        root.getChildren().add(btn);
        root.getChildren().add(label1);
        root.getChildren().add(textField);
        root.getChildren().add(IPText);
        Scene scene;
        scene = new Scene(root, 300, 250);
        primaryStage.setScene(scene);
        primaryStage.show();
        
        FXGameServer myServer = new FXGameServer();
        Thread test = new Thread(myServer);
        test.setDaemon(true);
        //test.start();
    }
    public Inet6Address getIPv6Addresses(InetAddress[] addresses) 
    {
        for (InetAddress addr : addresses) 
        {
//            System.out.println(addr.getHostAddress());
            if (addr instanceof Inet6Address) 
            {
                return (Inet6Address) addr;
            }
        }
        return null;
    }
}
