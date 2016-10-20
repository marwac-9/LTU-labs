/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.effect.Bloom;
import javafx.scene.effect.Glow;
import javafx.scene.layout.Pane;
import javafx.stage.Stage;

/**
 *
 * @author Deadline
 */
public class FXGameView extends Stage {

    Button refreshBtn = new Button();
    Bloom bloom = new Bloom();
    Glow glow = new Glow(0.9);
    boolean left = false, right = false, up = false , down = false, refresh = false, close = false;
    Scene scene;
    Pane root = new Pane();
    int speed = 5;
    TableView<ServerInfo> ServerTable = new TableView();
    ObservableList<ServerInfo> ServerList = FXCollections.observableArrayList();
    static final String Server[] = {"Name", "IP", "Port"};
    
    FXGameView()
    {
        refreshBtn.setText("Refresh");
        refreshBtn.setLayoutX(455);
        refreshBtn.setLayoutY(50);
        
        ServerTable.setMinSize(400, 300);
        ServerTable.setMaxSize(400, 600);
        TableColumn col_id = new TableColumn("ID");
        col_id.prefWidthProperty().bind(ServerTable.widthProperty().divide(100/13));
        ServerTable.getColumns().add(col_id);
        col_id.setCellValueFactory(new PropertyValueFactory<ServerInfo, String>("id"));
        
        TableColumn col_name = new TableColumn("Name");
        ServerTable.getColumns().add(col_name);
        col_name.setCellValueFactory(new PropertyValueFactory<ServerInfo, String>("Name"));
        col_name.prefWidthProperty().bind(ServerTable.widthProperty().divide(100/33));
        
        TableColumn col_ip = new TableColumn("IP");
        ServerTable.getColumns().add(col_ip);
        col_ip.setCellValueFactory(new PropertyValueFactory<ServerInfo, String>("IP"));
        col_ip.prefWidthProperty().bind(ServerTable.widthProperty().divide(100/33));
        
        TableColumn col_port = new TableColumn("Port");
        ServerTable.getColumns().add(col_port);
        col_port.setCellValueFactory(new PropertyValueFactory<ServerInfo, String>("Port"));
        col_port.prefWidthProperty().bind(ServerTable.widthProperty().divide(100/17));
        
        ServerTable.setItems(ServerList);
        ServerTable.setLayoutX(50);
        ServerTable.setLayoutY(50);
        ServerTable.setVisible(false);
        refreshBtn.setVisible(false);
        refreshBtn.setDisable(true);
        root.getChildren().add(refreshBtn);
        root.getChildren().add(ServerTable);
        root.setStyle("-fx-background-color:rgb(38, 38, 38);"); 

        
        this.scene = new Scene(root, 600, 500);
        
        
        //scene.getStylesheets().add("css/stylesheet.css");
        this.setTitle("Client");
        this.setScene(scene);

    }
    void addMovementOnKeyPress(EventHandler event)
    {
        this.scene.setOnKeyPressed(event);
    }
    void addMovementOnKeyRelease(EventHandler event)
    {
        this.scene.setOnKeyReleased(event);
    }
    void addRefreshOnButonPress(EventHandler event)
    {
        this.refreshBtn.setOnAction(event);
    }
    void addActionOnRowDoubleClick(EventHandler event)
    {
        this.ServerTable.setOnMouseClicked(event);
    }
    public boolean isUp()
    {
        return this.up;
    }
    public boolean isDown()
    {
        return this.down;
    }
    public boolean isLeft()
    {
        return this.left;
    }
    public boolean isRight()
    {
        return this.right;
    }
    public boolean isRefresh()
    {
        return this.refresh;
    }
    public boolean isClose()
    {
        return this.close;
    }
    public void setRefresh(boolean refr)
    {
        this.refresh = refr;
    }
    public void showTable()
    {
        this.ServerTable.setVisible(true);
        this.refreshBtn.setVisible(true);
        this.refreshBtn.setDisable(false);
    }
    public void hideTable()
    {
        this.ServerTable.setVisible(false);
        this.refreshBtn.setVisible(false);
        this.refreshBtn.setDisable(true);
    }
    public void addEventOnClose(EventHandler event)
    {
        this.setOnCloseRequest(event); 
    }
}
