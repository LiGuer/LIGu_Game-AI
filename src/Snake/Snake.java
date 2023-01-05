import java.util.*;

public class Snake {
	private LinkedList<Node> snakeBody;
	private int direction = 0; //0 up,1 down,2 left,3 right;
	public final int GRID_SIZE = 20;
	
	public Snake(int X,int Y){
		snakeBody = new LinkedList<Node>();
		snakeBody.add(new Node(X,Y));
		snakeBody.addLast(new Node(X+20,Y));
		snakeBody.addLast(new Node(X+20*2,Y));
	}
	
	public int getDirection() {
		return direction;
	}
	public void setDirection(int direction) {
		this.direction=direction;
	}
	
	public Node getSnakeHead() {
		return snakeBody.getFirst();
	}
	public Node getSnakeTail() {
		return snakeBody.getLast();
	}
	public LinkedList<Node> getSnakeBody() {
		return snakeBody;
	}
	
	public void snakeMove() {
		switch(direction) {
		case 0:
				snakeBody.addFirst(new Node(getSnakeHead().getNodeX(),getSnakeHead().getNodeY()-GRID_SIZE));
				break;
		case 1:
				snakeBody.addFirst(new Node(getSnakeHead().getNodeX(),getSnakeHead().getNodeY()+GRID_SIZE));
				break;
		case 2:
				snakeBody.addFirst(new Node(getSnakeHead().getNodeX()-GRID_SIZE,getSnakeHead().getNodeY()));
				break;
		case 3:
				snakeBody.addFirst(new Node(getSnakeHead().getNodeX()+GRID_SIZE,getSnakeHead().getNodeY()));
				break;
		}
		snakeBody.removeLast();
		System.out.printf("Snake Head :  %d %d\n",getSnakeBody().get(0).getNodeX(),getSnakeBody().get(0).getNodeY());
	}
	
	public void eatEgg(Node egg) {
		if(getSnakeHead().getNodeX()==egg.getNodeX()&&getSnakeHead().getNodeY()==egg.getNodeY()) {
			snakeBody.add(egg);
			setEgg(egg);
			System.out.printf("Egg :  %d %d\n",egg.getNodeX(),egg.getNodeY());
			System.out.println("Eat Egg !");
		}
	}
	public void setEgg(Node egg) {
		int t_X=new Random().nextInt(new SnakeGameView().ViewSize()/20)*GRID_SIZE,
				t_Y=new Random().nextInt(new SnakeGameView().ViewSize()/20)*GRID_SIZE;
		while(judgeBody(t_X,t_Y)) {
			t_X=new Random().nextInt(new SnakeGameView().ViewSize()/20)*GRID_SIZE;
			t_Y=new Random().nextInt(new SnakeGameView().ViewSize()/20)*GRID_SIZE;
		}
		egg.setNodeX(t_X);
		egg.setNodeY(t_Y);
	}
	
	public boolean inpactBorder() {
		if(getSnakeHead().getNodeX()<0 || getSnakeHead().getNodeY()<0
				|| getSnakeHead().getNodeX()>new SnakeGameView().ViewSize()-GRID_SIZE|| getSnakeHead().getNodeY()>new SnakeGameView().ViewSize()-GRID_SIZE) {
			System.out.println("Break");
			return false;
		}
		return true;
	}
	
	public boolean inpactBody() {
		if(judgeBody(getSnakeHead().getNodeX(),getSnakeHead().getNodeY())) {
			System.out.println("Break");
			return false;
		}
		return true;
	}
	
	public boolean judgeBody(int X,int Y) {
		for(int i=1;i<getSnakeBody().size();i++) {
			if(getSnakeBody().get(i).getNodeX()==X && getSnakeBody().get(i).getNodeY()==Y) {
				return true;
			}
		}
		return false;
	}
}