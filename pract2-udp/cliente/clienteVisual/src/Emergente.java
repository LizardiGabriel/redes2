import javax.swing.*;

public class Emergente{

    JFrame frame = new JFrame("mostrar");
    JPanel panel = new JPanel();
    JLabel label = new JLabel();
    JButton boton = new JButton();



    public Emergente(){


        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(300, 200);


        boton.setText("Aceptar");
        boton.setEnabled(false);
        boton.addActionListener(e -> frame.dispose());



        panel.add(label);
        panel.add(boton);
        frame.add(panel);
        frame.setVisible(true);




    }


    public void mostrar(int mensaje){
        System.out.println("<=== \t entre");
        label.setText(String.valueOf(mensaje));
        if (mensaje > 99){
            boton.setEnabled(true);
        }

    }
}
