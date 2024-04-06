import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JTextArea;
import java.awt.*;

public class Ventana {

    public void Ventana() {
        JFrame frame = new JFrame("Ventana");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        int height = getHeigh() /2;
        int width = getWidth()/2;


        int x = width / 20; // División más grande para ajustar tamaños
        int y = height / 20; // División más grande para ajustar tamaños

        frame.setSize(width, height); // Tamaño del marco ajustado

        JPanel panel = new JPanel();
        panel.setLayout(null);

        JTextField pathServidor = new JTextField();
        JTextField pathLocal = new JTextField();
        JTextArea listServidor = new JTextArea();
        JTextArea listLocal = new JTextArea();
        JTextField nombreFile = new JTextField();

        JButton mkdirServidor = new JButton("Crear carpeta");
        JButton mkdirLocal = new JButton("Crear carpeta");
        JButton rmdirServidor = new JButton("Eliminar");
        JButton rmdirLocal = new JButton("Eliminar");
        JButton cdServidor = new JButton("cambiar carpeta ");
        JButton cdLocal = new JButton("cambiar  carpeta");
        JButton put = new JButton("Enviar al server");
        JButton get = new JButton("Recibir del server");
        JButton ok = new JButton("OK");



        pathServidor.setBounds(x, y, 7 * x, y);
        pathLocal.setBounds(12*x, y, 7 * x, y);

        listServidor.setBounds(x, 3 * y, 7 * x, 10 * y);
        listLocal.setBounds(12*x, 3* y, 7 * x, 10 * y);

        mkdirServidor.setBounds(3*x, 13 * y, 3 * x, y);
        mkdirLocal.setBounds(14*x, 13 * y, 3 * x, y);

        cdServidor.setBounds(3*x, 14 * y, 3 * x, y);
        cdLocal.setBounds(14 *x, 14 * y, 3 * x, y);

        rmdirServidor.setBounds(3*x, 15 * y, 3 * x, y);
        rmdirLocal.setBounds(14*x, 15 * y, 3 * x, y);

        put.setBounds(8 * x, 14 * y, 4 * x, y);
        get.setBounds(8 * x, 15 * y, 4 * x, y);

        nombreFile.setBounds(8 * x, 16 * y, 3 * x, y);
        ok.setBounds(11 * x, 16 * y,  1* x, y);




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


        frame.add(panel);
        frame.setVisible(true);
    }

    public int getHeigh() {
        return Toolkit.getDefaultToolkit().getScreenSize().height;
    }

    public int getWidth() {
        return Toolkit.getDefaultToolkit().getScreenSize().width;
    }
}
