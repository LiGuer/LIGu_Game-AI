import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.*;
import java.applet.*;

public class SnakeGameView extends JFrame{
	
	private Snake snake;
	private Node egg;
	private boolean Flag = true;
	public final int GRID_SIZE = 20;
	public final int VIEW_WIDTH = ViewSize();
	
	public SnakeGameView() {
		snake = new Snake(200,200);
		egg = new Node(0,0);
	}
	public static void main(String[] args) {
		new SnakeGameView().showView();
	}
	
	public int ViewSize() {
		Toolkit kit = Toolkit.getDefaultToolkit();
		return kit.getScreenSize().height/2-kit.getScreenSize().height/2%20-2;
	}
	public void showView() {
		this.setBounds(ViewSize()/2,ViewSize()/2, VIEW_WIDTH, VIEW_WIDTH+30-2);
		this.setTitle("Snake");
		Image img = new ImageIcon("C:\\Users\\ÐÜè÷ºÀ\\Desktop\\Snake\\image\\logo.jpg").getImage();
		this.setIconImage(img);
		
		snake.setEgg(egg);
		this.add(new DrawView(snake,egg));
		this.setVisible(true);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.addKeyListener(new GameControl(snake));
		while(Flag) {
			snake.snakeMove();
			DrawView map = new DrawView(snake,egg);
			this.add(map);
			map.setBackground(new Color(250,250,255));
			this.setVisible(true);
			snake.eatEgg(egg);
			Flag=snake.inpactBorder();if(Flag==false)break;
			Flag=snake.inpactBody();if(Flag==false)break;
			try {Thread.sleep(140);}
			catch (InterruptedException e) {e.printStackTrace();}
		}
		this.add(new Score(snake));
		this.setVisible(true);
	}
	public void setFlag(boolean Flagtemp) {
		this.Flag=Flagtemp;
	}
}
