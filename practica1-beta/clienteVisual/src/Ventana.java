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



        JFrame frame = new JFrame("Ventana");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        int height = 2 * (getHeigh() / 3) ;
        int width = 2 * (getWidth()/3);

        frame.setSize(width, height); // Tamaño del marco ajustado


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


        textAreaLocal.setText(local.imprimirDirectorio(localPath, "  "));
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

    public void eventitos(){
        ok.addActionListener(e -> {
            consola.setText("ok");
        });


        mkdirServidor.addActionListener(e -> {

            textoAux(true);
            textoOk.setText("carpeta en servidor:");
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
            textoOk.setText("carpeta en local:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = local.crearCarpeta(localPath, nombreFile.getText());
                textAreaLocal.setText(local.imprimirDirectorio(localPath, "  "));
                textoAux(false);
            });
        });
        rmdirServidor.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("eliminar carpeta del servidor:");
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
            textoOk.setText("eliminar carpeta local:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = local.eliminarCarpeta(localPath, nombreFile.getText());
                textAreaLocal.setText(local.imprimirDirectorio(localPath, "  "));
                textoAux(false);
            });

        });
        cdServidor.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("cambiar directorio serv:");
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
            textoOk.setText("cambiar dir local:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret[] = local.cambiarDirectorio(localPath, nombreFile.getText());
                consola.setText(ret[0]);
                localPath = ret[1];
                pathLocal.setText(localPath);
                textAreaLocal.setText(local.imprimirDirectorio(localPath, "  "));

                textoAux(false);
            });


        });

        // logica para enviar archivo al servidor
        put.addActionListener(e -> {
            textoAux(true);
            textoOk.setText("file/fldr a enviar:");
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
            textoOk.setText("file/fldr a recivir:");
            ok.removeActionListener(ok.getActionListeners()[0]);
            ok.addActionListener(e1 -> {
                String ret = remoto.recibirArchivo(localPath, nombreFile.getText());
                consola.setText(ret);
                textAreaLocal.setText(local.imprimirDirectorio(localPath, "  "));
                textoAux(false);
            });
        });
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

        put.setBounds(8 * x, 13 * y, 4 * x, y);
        get.setBounds(8 * x, 14 * y, 4 * x, y);

        nombreFile.setBounds(8 * x, 16 * y, 3 * x, y);
        ok.setBounds(11 * x, 16 * y,  1* x, y);
        textoOk.setBounds(8 * x, 15 * y,  4* x, y);

        labelConsola.setBounds(x, 17 * y, 8 * x, y);
        consola.setBounds(x, 18 * y, 18 * x, 5*y);








    }

    public void cambiarColores(){


        Color ded = new Color(33, 40, 54);
        Color colorTexto = new Color(215, 195, 195);
        Color fondo = new Color(28, 29, 31);

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
        consola.setForeground(colorTexto);
        nombreFile.setForeground(colorTexto);

        nombreFile.setBorder(BorderFactory.createLineBorder(colorTexto));


        panel.setBackground(fondo);



        mkdirServidor.setForeground(colorTexto);
        mkdirServidor.setBorder(BorderFactory.createLineBorder(colorTexto));
        mkdirLocal.setForeground(colorTexto);
        mkdirLocal.setBorder(BorderFactory.createLineBorder(colorTexto));

        rmdirServidor.setForeground(colorTexto);
        rmdirServidor.setBorder(BorderFactory.createLineBorder(colorTexto));
        rmdirLocal.setForeground(colorTexto);
        rmdirLocal.setBorder(BorderFactory.createLineBorder(colorTexto));

        cdServidor.setForeground(colorTexto);
        cdServidor.setBorder(BorderFactory.createLineBorder(colorTexto));
        cdLocal.setForeground(colorTexto);
        cdLocal.setBorder(BorderFactory.createLineBorder(colorTexto));

        put.setForeground(colorTexto);
        put.setBorder(BorderFactory.createLineBorder(colorTexto));
        get.setForeground(colorTexto);
        get.setBorder(BorderFactory.createLineBorder(colorTexto));

        ok.setForeground(colorTexto);
        ok.setBorder(BorderFactory.createLineBorder(colorTexto));








    }


}
