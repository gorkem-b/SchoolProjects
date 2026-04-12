import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class PokemonQuestion {
    private static final int ANSWER_POOL_SIZE = 4;
    private static final int ADDITIONAL_WRONG_ANSWERS = 3;

    private final String imgPath;
    private final String correctAnswer;
    private final List<String> answerList;

    public String getImgPath() {
        return imgPath;
    }

    public String getCorrectAnswer() {
        return correctAnswer;
    }

    public List<String> getAnswerList() {
        return answerList;
    }

    public PokemonQuestion(int index) {
        this.imgPath = "resources/pokemon" + (index + 1) + ".png";
        this.correctAnswer = PokemonDB.POKEMON_DB[index];
        this.answerList = loadAnswerPool();
    }

    private List<String> loadAnswerPool() {
        List<String> answers = new ArrayList<>();
        Random rand = new Random();

        answers.add(correctAnswer);

        for (int i = 0; i < ADDITIONAL_WRONG_ANSWERS; i++) {
            String option = PokemonDB.POKEMON_DB[rand.nextInt(PokemonDB.POKEMON_DB.length)];
            while (answers.contains(option)) {
                option = PokemonDB.POKEMON_DB[rand.nextInt(PokemonDB.POKEMON_DB.length)];
            }
            answers.add(option);
        }

        randomizeAnswer(answers, rand.nextInt(answers.size()));
        return answers;
    }

    private void randomizeAnswer(List<String> answerList, int randIndex) {
        if (randIndex == 0) return;

        answerList.set(0, answerList.get(randIndex));
        answerList.set(randIndex, correctAnswer);
    }
}
