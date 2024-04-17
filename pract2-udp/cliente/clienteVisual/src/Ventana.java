import javax.swing.*;
import java.awt.*;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.IOException;

public class Ventana {

    JPanel panel = new JPanel();

    JTextField pathLocal = new JTextField();
    JTextField pathServidor = new JTextField();

    JTextArea textAreaLocal = new JTextArea();
    JTextArea textAreaServidor = new JTextArea();


    JScrollPane listLocal = new JScrollPane(textAreaLocal);
    JScrollPane listServidor = new JScrollPane(textAreaServidor);

    JTextArea consola = new JTextArea();

    JButton mkdirLocal = new JButton("Crear carpeta");
    JButton mkdirServidor = new JButton("Crear carpeta");
    JButton rmdirLocal = new JButton("Eliminar");
    JButton rmdirServidor = new JButton("Eliminar");
    JButton cdLocal = new JButton("cambiar  carpeta");
    JButton cdServidor = new JButton("cambiar carpeta ");
    JButton put = new JButton("Enviar al server");
    JButton get = new JButton("Recibir del server");

    JTextField nombreFile = new JTextField();
    JLabel textoOk = new JLabel("abcdefghijklmnopqrs");
    JButton ok = new JButton("OK");

    JLabel labelPathLocal = new JLabel("Ruta Local:");
    JLabel labelPathServidor = new JLabel("Ruta Servidor:");

    JLabel labelContenidoLocal = new JLabel("Contenido de ruta Local:");
    JLabel labelContenidoServidor = new JLabel("Contenido de ruta Servidor:");

    JLabel labelConsola = new JLabel("Consola:");

    String localPath = System.getProperty("user.dir");

    Local local = new Local();
    Remoto remoto = new Remoto();


    public void Ventana() throws IOException {



        JFrame frame = new JFrame("Cliente FTP");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        int height = 2 * (getHeigh() / 3) ;
        int width = 2 * (getWidth()/3);

        frame.setBounds(getWidth()/6, getHeigh()/6, width, height);


        panel.setLayout(null);


        textoAux(false);

        ajustarComponentes(height, width);
        frame.addComponentListener(new ComponentAdapter() {
            public void componentResized(ComponentEvent componentEvent) {
                int height1 = frame.getHeight();
                int width1 = frame.getWidth();

                ajustarComponentes(height1, width1);
            }
        });




        pathLocal.setText(localPath);
        pathServidor.setText(remoto.getDirectorio());


        textAreaLocal.setText(getDirectorio());
        textAreaServidor.setText(remoto.listarContenido());

        eventitos();

        pathServidor.setEditable(false);
        pathLocal.setEditable(false);
        textAreaLocal.setEditable(false);
        textAreaServidor.setEditable(false);
        consola.setEditable(false);




        panel.add(listServidor);
        panel.add(listLocal);
        panel.add(pathServidor);
        panel.add(pathLocal);
        panel.add(mkdirServidor);
        panel.add(mkdirLocal);
        panel.add(rmdirServidor);
        panel.add(rmdirLocal);
        panel.add(cdServidor);
        panel.add(cdLocal);
        panel.add(put);
        panel.add(get);
        panel.add(nombreFile);
        panel.add(ok);
        panel.add(textoOk);
        panel.add(consola);

        panel.add(labelPathLocal);
        panel.add(labelPathServidor);
        panel.add(labelConsola);
        panel.add(labelContenidoLocal);
        panel.add(labelContenidoServidor);



        frame.add(panel);

        cambiarColores();

        frame.setVisible(true);

    }

    private void textoAux(boolean b) {
        nombreFile.setVisible(b);
        textoOk.setVisible(b);
        ok.setVisible(b);

        nombreFile.setText("");


    }

    public int getHeigh() {
        return Toolkit.getDefaultToolkit().getScreenSize().height;
    }

    public int getWidth() {
        return Toolkit.getDefaultToolkit().getScreenSize().width;
    }

    public void mensaje(String mensaje) {
        JOptionPane.showMessageDialog(null, mensaje);
    }
    public int seleccionBuff(long tam, String msg){

        int ret = 1;
        // preguntar el tamaño del buffer

        String res = JOptionPane.showInputDialog("Tam del archivo: "+ tam +". Tamaño del buffer: " );
        try {
            ret = Integer.parseInt(res);
            if(ret > tam){
                mensaje("Error: el tamaño del buffer no puede ser mayor al tamaño del archivo.");
                return seleccionBuff(tam, msg);
            }
            if (ret < 1){
                mensaje("Error: el tamaño del buffer debe ser mayor a 0.");
                return seleccionBuff(tam, msg);
            }

        } catch (NumberFormatException e) {
            mensaje("Error: el tamaño del buffer debe ser un número entero.");
            return seleccionBuff(tam, msg);
        }
        return ret;
    }
    public int seleccionVentana(long tam, int tamBuffer, String msg){

        int ret = 1;
        // preguntar el tamaño del buffer
        String res = JOptionPane.showInputDialog("Tam de la ventana:");

        try {
            ret = Integer.parseInt(res);
            int MaxVentana = (int) Math.ceil((double) tam / tamBuffer);
            if(ret > MaxVentana){
                mensaje("Error: el tamaño de la ventana no puede ser mayor a " + MaxVentana);
                return seleccionVentana(tam, tamBuffer, msg);
            }
            if (ret < 1){
                mensaje("Error: el tamaño de la ventana debe ser mayor a 0.");
                return seleccionVentana(tam, tamBuffer, msg);
            }


        } catch (NumberFormatException e) {
            mensaje("Error: el tamaño del buffer debe ser un número entero.");
            return seleccionVentana(tam, tamBuffer, msg);
        }
        return ret;
    }


    public void eventitos(){
        ok.addActionListener(e -> {
            consola.setText("ok");
        });


        mkdirServidor.addActionListener(e -> {

            textoAux(true);
            textoOk.setText("(serv) crear carpeta:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = remoto.crearCarpeta(nombreFile.getText());
                consola.setText(ret);
                try {
                    textAreaServidor.setText(remoto.listarContenido());
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }
                textoAux(false);

            });


        });
        mkdirLocal.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("(local) crear carpeta:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = local.crearCarpeta(localPath, nombreFile.getText());
                textAreaLocal.setText(getDirectorio());
                textoAux(false);
            });
        });
        rmdirServidor.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("(serv) eliminar:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                        String ret = remoto.eliminarAlgo(nombreFile.getText());
                        consola.setText(ret);
                        try {
                            textAreaServidor.setText(remoto.listarContenido());
                        } catch (IOException ex) {
                            throw new RuntimeException(ex);
                        }
                        textoAux(false);
                    }
            );
        });
        rmdirLocal.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("(local) eliminar:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = local.eliminarCarpeta(localPath, nombreFile.getText());
                consola.setText(ret);
                textAreaLocal.setText(getDirectorio());
                textoAux(false);
            });

        });
        cdServidor.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("(serv) folder:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = remoto.cambiarDirectorio(nombreFile.getText());
                consola.setText(ret);
                try {
                    textAreaServidor.setText(remoto.listarContenido());
                    pathServidor.setText(remoto.getDirectorio());
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }
                textoAux(false);
            });
        });

        cdLocal.addActionListener(o -> {
            textoAux(true);
            textoOk.setText("(local) folder:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret[] = local.cambiarDirectorio(localPath, nombreFile.getText());
                consola.setText(ret[0]);
                localPath = ret[1];
                pathLocal.setText(localPath);
                textAreaLocal.setText(getDirectorio());

                textoAux(false);
            });


        });

        // logica para enviar archivo al servidor
        put.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("enviar:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = remoto.enviarArchivo(localPath, nombreFile.getText());
                consola.setText(ret);
                try {
                    textAreaServidor.setText(remoto.listarContenido());
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }
                textoAux(false);
            });
        });

        get.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("recibir:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = remoto.recibirArchivo(localPath, nombreFile.getText());
                consola.setText(ret);
                textAreaLocal.setText(getDirectorio());
                textoAux(false);
            });
        });
    }


    public String getDirectorio() {
        return local.imprimirDirectorio(localPath, "  ");
    }
    public void ajustarComponentes(int height, int width) {

        int x = width / 20;
        int y = height / 25;


        labelPathServidor.setBounds(x, 0, 8 * x, y);
        labelPathLocal.setBounds(11*x, 0, 8 * x, y);

        pathServidor.setBounds(x, y, 8 * x, y);
        pathLocal.setBounds(11*x, y, 8 * x, y);

        labelContenidoServidor.setBounds(x, 2 * y, 8 * x, y);
        labelContenidoLocal.setBounds(12*x, 2 * y, 8 * x, y);

        listServidor.setBounds(x, 3 * y, 7 * x, 10 * y);
        listLocal.setBounds(12*x, 3* y, 7 * x, 10 * y);

        mkdirServidor.setBounds(3*x, 14 * y, 3 * x, y);
        mkdirLocal.setBounds(14*x, 14 * y, 3 * x, y);

        cdServidor.setBounds(3*x, 15 * y, 3 * x, y);
        cdLocal.setBounds(14 *x, 15 * y, 3 * x, y);

        rmdirServidor.setBounds(3*x, 16 * y, 3 * x, y);
        rmdirLocal.setBounds(14*x, 16 * y, 3 * x, y);

        put.setBounds(9 * x, 12 * y, 2 * x, y);
        get.setBounds(9 * x, 14 * y, 2 * x, y);

        nombreFile.setBounds(8 * x, 16 * y, 3 * x, y);
        ok.setBounds(11 * x, 16 * y,  1* x, y);
        textoOk.setBounds(8 * x, 15 * y,  4* x, y);

        labelConsola.setBounds(x, 17 * y, 8 * x, y);
        consola.setBounds(x, 18 * y, 18 * x, 5*y);


        // cambiar tam de letra
        int tamLetra = width/90;
        String letra = "Comic Sans MS";

        labelPathServidor.setFont(new Font(letra, Font.BOLD, tamLetra));
        labelPathLocal.setFont(new Font(letra, Font.BOLD, tamLetra));
        labelContenidoServidor.setFont(new Font(letra, Font.BOLD, tamLetra));
        labelContenidoLocal.setFont(new Font(letra, Font.BOLD, tamLetra));
        labelConsola.setFont(new Font(letra, Font.BOLD, tamLetra));
        textoOk.setFont(new Font(letra, Font.BOLD, tamLetra));

        pathServidor.setFont(new Font(letra, Font.PLAIN, tamLetra));
        pathLocal.setFont(new Font(letra, Font.PLAIN, tamLetra));
        textAreaLocal.setFont(new Font(letra, Font.PLAIN, tamLetra));
        textAreaServidor.setFont(new Font(letra, Font.PLAIN, tamLetra));
        consola.setFont(new Font(letra, Font.PLAIN, tamLetra));
        nombreFile.setFont(new Font(letra, Font.PLAIN, tamLetra));

        ok.setFont(new Font(letra, Font.PLAIN, tamLetra));
        mkdirLocal.setFont(new Font(letra, Font.PLAIN, tamLetra));
        mkdirServidor.setFont(new Font(letra, Font.PLAIN, tamLetra));
        rmdirLocal.setFont(new Font(letra, Font.PLAIN, tamLetra));
        rmdirServidor.setFont(new Font(letra, Font.PLAIN, tamLetra));
        cdLocal.setFont(new Font(letra, Font.PLAIN, tamLetra));
        cdServidor.setFont(new Font(letra, Font.PLAIN, tamLetra));
        put.setFont(new Font(letra, Font.PLAIN, tamLetra));
        get.setFont(new Font(letra, Font.PLAIN, tamLetra));

    }

    public void cambiarColores(){


        Color ded = new Color(38, 40, 42);
        Color colorTexto = new Color(215, 195, 195);
        Color fondo = new Color(28, 29, 31);
        Color textoConsola = new Color(126, 164, 81);

        labelPathLocal.setForeground(colorTexto);
        labelPathServidor.setForeground(colorTexto);
        labelContenidoLocal.setForeground(colorTexto);
        labelContenidoServidor.setForeground(colorTexto);
        labelConsola.setForeground(colorTexto);
        textoOk.setForeground(colorTexto);

        pathLocal.setBackground(ded);
        pathServidor.setBackground(ded);
        textAreaLocal.setBackground(ded);
        textAreaServidor.setBackground(ded);
        consola.setBackground(ded);
        nombreFile.setBackground(ded);

        pathLocal.setForeground(colorTexto);
        pathServidor.setForeground(colorTexto);
        textAreaLocal.setForeground(colorTexto);
        textAreaServidor.setForeground(colorTexto);
        consola.setForeground(textoConsola);
        nombreFile.setForeground(colorTexto);

        nombreFile.setBorder(BorderFactory.createLineBorder(colorTexto));


        panel.setBackground(fondo);

        Color colorcito = new Color(69, 71, 77);

        mkdirServidor.setOpaque(true);
        mkdirServidor.setBackground(colorcito);
        mkdirServidor.setForeground(colorTexto);
        mkdirServidor.setBorder(BorderFactory.createLineBorder(colorTexto));

        mkdirLocal.setOpaque(true);
        mkdirLocal.setBackground(colorcito);
        mkdirLocal.setForeground(colorTexto);
        mkdirLocal.setBorder(BorderFactory.createLineBorder(colorTexto));

        rmdirServidor.setOpaque(true);
        rmdirServidor.setBackground(colorcito);
        rmdirServidor.setForeground(colorTexto);
        rmdirServidor.setBorder(BorderFactory.createLineBorder(colorTexto));

        rmdirLocal.setOpaque(true);
        rmdirLocal.setBackground(colorcito);
        rmdirLocal.setForeground(colorTexto);
        rmdirLocal.setBorder(BorderFactory.createLineBorder(colorTexto));

        cdServidor.setOpaque(true);
        cdServidor.setBackground(colorcito);
        cdServidor.setForeground(colorTexto);
        cdServidor.setBorder(BorderFactory.createLineBorder(colorTexto));

        cdLocal.setOpaque(true);
        cdLocal.setBackground(colorcito);
        cdLocal.setForeground(colorTexto);
        cdLocal.setBorder(BorderFactory.createLineBorder(colorTexto));

        put.setOpaque(true);
        put.setBackground(colorcito);
        put.setForeground(colorTexto);
        put.setBorder(BorderFactory.createLineBorder(colorTexto));

        get.setOpaque(true);
        get.setBackground(colorcito);
        get.setForeground(colorTexto);
        get.setBorder(BorderFactory.createLineBorder(colorTexto));

        ok.setOpaque(true);
        ok.setBackground(colorcito);
        ok.setForeground(colorTexto);
        ok.setBorder(BorderFactory.createLineBorder(colorTexto));

    }


}
