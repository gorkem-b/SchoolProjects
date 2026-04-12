import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class PokemonQuiz extends JFrame implements ActionListener {
    private static final int QUESTION_AMOUNT = 10;

    private List<PokemonQuestion> questions;
    private List<JButton> answerButtons;
    private SpringLayout springLayout;
    private PokemonQuestion currentPokemonQuestion;
    private JLabel pokemonQuestionNumberLabel, currentPokemonImage;
    private JButton nextButton;
    private boolean firstChoice;
    private int currentPokemonQuestionNum, score;

    public PokemonQuiz(){
        super("Pokemon Scarlet and Violet Quiz");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setPreferredSize(new Dimension(800, 800));
        pack();
        setLocationRelativeTo(null);
        setResizable(false);

        loadQuestions();

        // init vars
        firstChoice = false;
        score = 0;
        springLayout = new SpringLayout();
        currentPokemonQuestionNum = 0;
        currentPokemonQuestion = questions.get(currentPokemonQuestionNum);

        addGuiComponents();
    }

    private void addGuiComponents(){
        JPanel jPanel = new JPanel();
        jPanel.setLayout(springLayout);

        //1. Banner Image
        JLabel bannerImg = ImgService.loadImage("resources/banner.png", true, 600, 100);
        jPanel.add(bannerImg);
        springLayout.putConstraint(SpringLayout.WEST, bannerImg, 100, SpringLayout.WEST, jPanel);
        springLayout.putConstraint(SpringLayout.NORTH, bannerImg, 25, SpringLayout.NORTH, jPanel);

        //2. Number Label
        pokemonQuestionNumberLabel = new JLabel((currentPokemonQuestionNum + 1) + ".");
        pokemonQuestionNumberLabel.setFont(new Font("Dialog", Font.BOLD, 18));
        jPanel.add(pokemonQuestionNumberLabel);
        springLayout.putConstraint(SpringLayout.WEST, pokemonQuestionNumberLabel, 100, SpringLayout.WEST, jPanel);
        springLayout.putConstraint(SpringLayout.NORTH, pokemonQuestionNumberLabel, 175, SpringLayout.NORTH, jPanel);

        //3 Pokemon Image
        currentPokemonImage = ImgService.loadImage(currentPokemonQuestion.getImgPath(), true, 250, 250);
        jPanel.add(currentPokemonImage);
        springLayout.putConstraint(SpringLayout.WEST, currentPokemonImage, 275, SpringLayout.WEST, jPanel);
        springLayout.putConstraint(SpringLayout.NORTH, currentPokemonImage, 175, SpringLayout.NORTH, jPanel);

        //4. Answer Buttons
        JPanel answerPanel = loadButtons();
        jPanel.add(answerPanel);
        springLayout.putConstraint(SpringLayout.WEST,answerPanel, 80, SpringLayout.WEST, jPanel);
        springLayout.putConstraint(SpringLayout.NORTH, answerPanel, 450, SpringLayout.NORTH, jPanel);

        //5. Reset Button
        JButton resetButton = ImgService.createImageButton("resources/resetImg.png", "Reset");
        resetButton.setToolTipText("Resets Quiz");
        resetButton.setPreferredSize(new Dimension(60, 50));
        resetButton.addActionListener(this);
        resetButton.setOpaque(false);
        resetButton.setBorderPainted(false);
        jPanel.add(resetButton);
        springLayout.putConstraint(SpringLayout.WEST,resetButton, 15, SpringLayout.WEST, jPanel);
        springLayout.putConstraint(SpringLayout.NORTH, resetButton, 50, SpringLayout.NORTH, jPanel);

        //6. Next Button
        nextButton = ImgService.createImageButton("resources/nextImg.png", "Next");
        nextButton.setToolTipText("Goes to the next question");
        nextButton.setPreferredSize(new Dimension(45, 50));
        nextButton.addActionListener(this);
        nextButton.setOpaque(false);
        nextButton.setContentAreaFilled(false);
        nextButton.setVisible(false); // only appear after user has pressed a button
        jPanel.add(nextButton);
        springLayout.putConstraint(SpringLayout.WEST,nextButton, 725, SpringLayout.WEST, jPanel);
        springLayout.putConstraint(SpringLayout.NORTH, nextButton, 675, SpringLayout.NORTH, jPanel);


        this.getContentPane().add(jPanel);
    }

    private JPanel loadButtons(){
        JPanel buttonPanel = new JPanel();
        GridLayout gridLayout = new GridLayout(2, 2);
        buttonPanel.setLayout(gridLayout);
        gridLayout.setHgap(25);
        gridLayout.setVgap(25);

        answerButtons = new ArrayList<>();

        for(int i = 0; i < 4; i++){
            JButton button = new JButton(currentPokemonQuestion.getAnswerList().get(i));
            button.addActionListener(this);
            button.setFont(new Font("Dialog", Font.BOLD, 18));
            button.setPreferredSize(new Dimension(300, 100));

            answerButtons.add(button);
            buttonPanel.add(answerButtons.get(i));
        }
        return buttonPanel;
    }

    private void loadQuestions(){
        questions = new ArrayList<PokemonQuestion>();

        // create and store the pokemone questions
        for(int i = 0; i < QUESTION_AMOUNT; i++){
            questions.add(new PokemonQuestion(i));
        }

        // randomize questions
        for(int i = 0; i < QUESTION_AMOUNT; i++){
            int randIndex = new Random().nextInt(QUESTION_AMOUNT);
            PokemonQuestion temp = questions.get(i);
            questions.set(i, questions.get(randIndex));
            questions.set(randIndex, temp);
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        String command = e.getActionCommand();
        if(command.equals("Reset")){
            // reset score
            score = 0;

            // reset question number
            currentPokemonQuestionNum = 0;

            // load a new batch of questions
            loadQuestions();

            // update gui
            updateGui();

        }else if(command.equals("Next")){
            nextButton.setVisible(false);
            firstChoice = false;

            // go to the next question
            currentPokemonQuestionNum++;

            // update gui
            updateGui();

        }else{ // Pokemon Answer Button
            JButton button = (JButton) e.getSource();
            if(currentPokemonQuestion.getCorrectAnswer().equals(command)){
                // indicate that the answer was correct
                button.setBackground(Color.GREEN);

                // update score
                if(!firstChoice){
                    ++score;
                }

            }else{
                button.setBackground(Color.RED);
            }
            if(currentPokemonQuestionNum < QUESTION_AMOUNT - 1 && !nextButton.isVisible()){
                nextButton.setVisible(true);
            }

            firstChoice = true;
        }

        // display score to the user
        if(firstChoice && currentPokemonQuestionNum == QUESTION_AMOUNT - 1){
            launchFinishedDialog();
        }

    }

    private void updateGui(){
        // reset color of buttons
        for(int i = 0; i < answerButtons.size(); i++){
            answerButtons.get(i).setBackground(null);
        }

        // updates the question number
        pokemonQuestionNumberLabel.setText((currentPokemonQuestionNum + 1) + ".");

        // update pokemon question img and answers
        currentPokemonQuestion = questions.get(currentPokemonQuestionNum);
        ImgService.updateImage(currentPokemonImage, currentPokemonQuestion.getImgPath(), true, 250, 250);
        for(int i = 0; i < 4; i++){
            answerButtons.get(i).setText(currentPokemonQuestion.getAnswerList().get(i));
        }
    }
    private void launchFinishedDialog(){
        JDialog finished = new JDialog();
        finished.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
        finished.setPreferredSize(new Dimension(400, 200));
        finished.pack();
        finished.setLayout(springLayout);
        finished.setLocationRelativeTo(this);

        JLabel scoreResult = new JLabel("Score: " + (score) + "/" + QUESTION_AMOUNT);
        scoreResult.setFont(new Font("Dialog", Font.BOLD, 18));
        springLayout.putConstraint(SpringLayout.WEST,scoreResult, 135, SpringLayout.WEST, finished);
        springLayout.putConstraint(SpringLayout.NORTH, scoreResult, 65, SpringLayout.NORTH, finished);

        finished.add(scoreResult);
        finished.setVisible(true);
    }
}


