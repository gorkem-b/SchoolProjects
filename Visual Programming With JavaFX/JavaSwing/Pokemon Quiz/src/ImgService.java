import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;

public class ImgService {
    public static void updateImage(JLabel imageContainer, String resourcePath, boolean isResized, int targetWidth, int targetHeight) {
        try {
            BufferedImage image = loadImageFromResource(resourcePath);
            if (isResized) {
                image = resizeImage(image, targetWidth, targetHeight);
            }
            imageContainer.setIcon(new ImageIcon(image));
        } catch (IOException e) {
            System.err.println("Failed to load image from " + resourcePath + ": " + e.getMessage());
        }
    }

    public static JButton createImageButton(String resourcePath, String command) {
        try {
            BufferedImage image = loadImageFromResource(resourcePath);
            JButton imageButton = new JButton(command);
            imageButton.setIcon(new ImageIcon(image));
            return imageButton;
        } catch (IOException e) {
            System.err.println("Failed to create image button from " + resourcePath + ": " + e.getMessage());
            return null;
        }
    }

    public static JLabel loadImage(String resourcePath, boolean isResized, int targetWidth, int targetHeight) {
        try {
            BufferedImage image = loadImageFromResource(resourcePath);
            if (isResized) {
                image = resizeImage(image, targetWidth, targetHeight);
            }
            return new JLabel(new ImageIcon(image));
        } catch (IOException e) {
            System.err.println("Failed to load image from " + resourcePath + ": " + e.getMessage());
            return null;
        }
    }

    private static BufferedImage loadImageFromResource(String resourcePath) throws IOException {
        InputStream inputStream = ImgService.class.getResourceAsStream(resourcePath);
        if (inputStream == null) {
            throw new IOException("Resource not found: " + resourcePath);
        }
        return ImageIO.read(inputStream);
    }

    private static BufferedImage resizeImage(BufferedImage image, int targetWidth, int targetHeight) {
        BufferedImage newImage = new BufferedImage(targetWidth, targetHeight, BufferedImage.TYPE_INT_RGB);
        Graphics2D graphics2D = newImage.createGraphics();
        graphics2D.drawImage(image, 0, 0, targetWidth, targetHeight, null);
        graphics2D.dispose();
        return newImage;
    }
}
