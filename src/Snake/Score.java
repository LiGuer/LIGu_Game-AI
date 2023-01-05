import java.awt.*;
import javax.swing.*;

public class Score extends JPanel {
	
	private Snake snake;
	private int scoreSnake;
	
	public Score(Snake snake) {
		this.snake = snake;
		scoreSnake = snake.getSnakeBody().size()-3;
	}
	
	public void paint(Graphics g) {
		String scoreans = String.valueOf(scoreSnake);
		g.setFont(new Font("Stencil Std", Font.PLAIN, 50));
		g.setColor(Color.black);
		g.drawString("Your Score is :" ,new SnakeGameView().ViewSize()/13,new SnakeGameView().ViewSize()/2-new SnakeGameView().ViewSize()/5);
		g.setFont(new Font("Stencil Std", Font.PLAIN, 70));
		g.setColor(Color.red);
		g.drawString(scoreans ,new SnakeGameView().ViewSize()/2-new SnakeGameView().ViewSize()/10
				,new SnakeGameView().ViewSize()/2+new SnakeGameView().ViewSize()/10);
	}
}