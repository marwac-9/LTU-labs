/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

/**
 *
 * @author Deadline
 */
public class ServerInfo {
    private int id;
    private String Name;
    private String IP;
    private int Port;

    ServerInfo(int ID, String name, String ip, int port) {
        this.id = ID;
        this.Name = name;
        this.IP = ip;
        this.Port = port;
    }

    public int getId() {
        return id;
    }

    public void setId(int ID) {
        this.id = ID;
    }

    public String getName() {
        return Name;
    }

    public void setName(String name) {
        Name = name;
    }

    public String getIP() {
        return IP;
    }

    public void setIP(int ip) {
        Port = ip;
    }

    public int getPort() {
        return Port;
    }

    public void setPort(int port) {
        Port = port;
    }
    
}
