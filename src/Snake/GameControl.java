import javax.swing.*;
import java.awt.event.*;

public class GameControl implements KeyListener{
	private Snake snake;
	public GameControl(Snake snaketemp) {
		this.snake=snaketemp;
	}
	public void keyPressed(KeyEvent e) {
		switch(e.getKeyCode()) {
		case KeyEvent.VK_UP:
			if(snake.getDirection()==1)break;
			snake.setDirection(0);
			break;
		case KeyEvent.VK_DOWN:
			if(snake.getDirection()==0)break;
			snake.setDirection(1);
			break;
		case KeyEvent.VK_LEFT:
			if(snake.getDirection()==3)break;
			snake.setDirection(2);
			break;
		case KeyEvent.VK_RIGHT:
			if(snake.getDirection()==2)break;
			snake.setDirection(3);
			break;
		}
		System.out.printf("Snake Direction : %d\n",snake.getDirection());
	}
	public void keyReleased(KeyEvent arg0) {}
	public void keyTyped(KeyEvent arg0) {}
}
