import java.awt.*;
import javax.swing.*;

public class DrawView extends JPanel {
	
	public final int GRID_SIZE = 20;
	private Snake snake;
	private Node egg;
	
	public DrawView(Snake snake,Node egg) {
		this.snake = snake;
		this.egg=egg;
	}
	
	public void paint(Graphics g) {
		super.paint(g);
		drawEgg(g);
		drawSnake(g);
	}
	
	public void drawSnake(Graphics g) {
		for(int i=0;i<snake.getSnakeBody().size();i++) {
			g.setColor(Color.green);
			if(i==0)g.setColor(Color.yellow);
			g.fillRect(snake.getSnakeBody().get(i).getNodeX(), snake.getSnakeBody().get(i).getNodeY(), GRID_SIZE, GRID_SIZE);
		}
	}
	public void drawEgg(Graphics g) {
		g.setColor(Color.red);
		g.fillRect(this.egg.getNodeX(), this.egg.getNodeY(), GRID_SIZE, GRID_SIZE);
	}
}
