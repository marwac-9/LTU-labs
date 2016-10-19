/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

import javafx.scene.effect.Bloom;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;

/**
 *
 * @author Deadline
 */
public class PlayerInfo extends Rectangle
{
    long id;
    int speed;

    PlayerInfo(long id, int posx, int posy, int width, int height)
    {
        this.id = id;
        this.setX(0);
        this.setY(0);
        this.setLayoutX(posx);
        this.setLayoutY(posy);
        this.setWidth(width);
        this.setHeight(height);

        //all this stuff can be set by gui instead but whatever
        this.setArcWidth(20);
        this.setArcHeight(20);
        this.setFill(Color.rgb(127,255,255,0.6));
        this.setStroke(Color.WHITE);
        this.setStrokeWidth(3);
        this.setStrokeDashOffset(23);
        this.getStrokeDashArray().addAll(30d, 30d, 6d, 30d);
        Bloom bloom = new Bloom();
        bloom.setThreshold(0.1);
        this.setEffect(bloom);

    }
    PlayerInfo()
    {
    }
}
