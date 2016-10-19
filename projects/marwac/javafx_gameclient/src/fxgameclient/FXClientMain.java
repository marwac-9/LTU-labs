/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package fxgameclient;

import java.io.IOException;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.scene.chart.XYChart;
import javafx.scene.control.ContentDisplay;
import javafx.scene.control.TableCell;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TextField;
import javafx.stage.Stage;
import javafx.util.Callback;

/**
 *
 * @author Deadline
 */
public class FXClientMain extends Application{
    

    /**
     * start method is called by mains method launch<br>
     * in this case we are ignoring the primaryStage<br>
     * as we want to show our own view with extended functionality<br>
     * @param primaryStage stage not used by our application, instead we initialize own WeatherView2 class to show
     */
    @Override
    public void start(Stage primaryStage) throws IOException {
        
        
        Callback<TableColumn, TableCell> cellFactory =
        new Callback<TableColumn, TableCell>() {

            @Override
            public TableCell call(TableColumn p) {
                return new EditingCell();
            }
        };
        

        FXGameView gView = new FXGameView();
        //gView.show();
        FXController controller = new FXController(gView);
            
        Thread cntrollerThread = new Thread(controller);
        cntrollerThread.setDaemon(true);
        cntrollerThread.start();

        //FXController appController = new FXController(gView);

        //appController.startRunning();
    }
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        launch(args);        
        
    }

   
    
    class EditingCell extends TableCell<XYChart.Data, Number> {
          
        private TextField textField;
          
        public EditingCell() {}
          
          
        @Override
        public void updateItem(Number item, boolean empty) {
            super.updateItem(item, empty);
              
            if (empty) {
                setText(null);
                setGraphic(null);
            } else {
                if (isEditing()) {
                    if (textField != null) {
                        textField.setText(getString());
                    }
                    setGraphic(textField);
                    setContentDisplay(ContentDisplay.GRAPHIC_ONLY);
                } else {
                    setText(getString());
                    setContentDisplay(ContentDisplay.TEXT_ONLY);
                }
            }
        }
          
        
          
        private String getString() {
            return getItem() == null ? "" : getItem().toString();
        }
    }
    
}