/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameserver;

/**
 *
 * @author Deadline
 */
public class Player
    {
        long id;
        int posx;
        int posy;
        int width;
        int height;
        int speed;
        
        Player(long id, int posx, int posy, int speed, int width, int height)
        {
            this.id = id;
            this.posx = posx;
            this.posy = posy;
            this.width = width;
            this.height = height;
            this.speed = speed;
        }
        Player()
        {
            this.id = 0;
            this.posx = 50; //should be  be random in range
            this.posy = 50;
            this.width = 100;
            this.height = 100;
            this.speed = 5;
        }
        Player(long playerID)
        {
            this.id = playerID;
            this.posx = 50; //should be  be random in range
            this.posy = 50;
            this.width = 100;
            this.height = 100;
            this.speed = 5;
        }
    }