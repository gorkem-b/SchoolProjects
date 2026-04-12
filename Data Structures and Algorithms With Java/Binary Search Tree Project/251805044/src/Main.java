import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.Scanner;

public class Main {

    private static final String FILE_NAME = "inputBst.txt";
    private static final Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) {
        ProductBST tree = new ProductBST();

        loadInitialData(tree);
        runMainMenu(tree);

        scanner.close();
    }

    // ============ PROGRAM AKIŞI ============

    private static void runMainMenu(ProductBST tree) {
        while (true) {
            displayMenu();
            int userChoice = getUserChoice();

            if (userChoice == 0) {
                exitProgram();
                break;
            }

            handleMenuChoice(tree, userChoice);
        }
    }

    private static void displayMenu() {
        System.out.println("\n--- PRODUCT STOCK MENU ---");
        System.out.println("1 - Yeni ürün ekle / güncelle");
        System.out.println("2 - In-order yazdır");
        System.out.println("3 - Miktar artır");
        System.out.println("4 - Miktar azalt");
        System.out.println("5 - Miktara göre artan yazdır");
        System.out.println("0 - Çıkış");
        System.out.print("Seçiminiz: ");
    }

    private static int getUserChoice() {
        int choice = scanner.nextInt();
        scanner.nextLine();
        return choice;
    }

    private static void handleMenuChoice(ProductBST tree, int choice) {
        switch (choice) {
            case 1:
                handleInsertNewProduct(tree);
                break;
            case 2:
                handlePrintInOrder(tree);
                break;
            case 3:
                handleIncreaseQuantity(tree);
                break;
            case 4:
                handleDecreaseQuantity(tree);
                break;
            case 5:
                handlePrintByQuantity(tree);
                break;
            default:
                printInvalidChoice();
        }
    }

    // ============ DOSYA İŞLEMLERİ ============

    private static void loadInitialData(ProductBST tree) {
        try {
            readProductsFromFile(tree, FILE_NAME);
            printDataLoadedSuccessfully();
        } catch (IOException e) {
            printFileReadError(e);
        }
    }

    private static void readProductsFromFile(ProductBST tree, String filename) throws IOException {
        BufferedReader fileReader = new BufferedReader(new FileReader(filename));
        String currentLine;

        while ((currentLine = fileReader.readLine()) != null) {
            insertProductFromLine(tree, currentLine);
        }

        fileReader.close();
    }

    private static void insertProductFromLine(ProductBST tree, String line) {
        if (line.trim().isEmpty()) {
            return;
        }

        String[] productData = line.split(",");

        if (productData.length < 3) {
            System.out.println("Satır formatı hatalı, atlanıyor: " + line);
            return;
        }

        int barcode = Integer.parseInt(productData[0].trim());
        String name = productData[1].trim();
        int quantity = Integer.parseInt(productData[2].trim());

        tree.insert(barcode, name, quantity);
    }

    // BST'deki güncel tüm kayıtları inputBst.txt dosyasına yazar
    private static void saveData(ProductBST tree) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(FILE_NAME))) {
            List<Node> products = tree.toListInOrder(); // barkod sırasına göre
            for (Node product : products) {
                String line = product.barcodeNumber + "," +
                        product.productName + "," +
                        product.quantity;
                writer.write(line);
                writer.newLine();
            }
            System.out.println("Değişiklikler " + FILE_NAME + " dosyasına kaydedildi.");
        } catch (IOException e) {
            System.out.println(FILE_NAME + " yazılırken hata: " + e.getMessage());
        }
    }

    // ============ MENÜ İŞLEYİCİLERİ ============

    private static void handleInsertNewProduct(ProductBST tree) {
        int barcode = getBarcodeFromUser();
        String productName = getProductNameFromUser();
        int quantity = getQuantityFromUser();

        tree.insert(barcode, productName, quantity);
        printProductInsertedOrUpdated();
        saveData(tree);
    }

    private static void handlePrintInOrder(ProductBST tree) {
        tree.printInOrder();
    }

    private static void handleIncreaseQuantity(ProductBST tree) {
        int barcode = getBarcodeFromUser();
        int increaseAmount = getIncreaseAmountFromUser();

        boolean changed = tree.increaseQuantity(barcode, increaseAmount);

        if (changed) {
            saveData(tree);
            printCurrentStockStatus();
            tree.printInOrder();
        }
    }

    private static void handleDecreaseQuantity(ProductBST tree) {
        int barcode = getBarcodeFromUser();
        int decreaseAmount = getDecreaseAmountFromUser();

        boolean changed = tree.decreaseQuantity(barcode, decreaseAmount);

        if (changed) {
            saveData(tree);
            printCurrentStockStatus();
            tree.printInOrder();
        }
    }

    private static void handlePrintByQuantity(ProductBST tree) {
        tree.printByQuantityAscending();
    }

    // ============ KULLANICI GİRİŞLERİ ============

    private static int getBarcodeFromUser() {
        System.out.print("Barkod numarası: ");
        int barcode = scanner.nextInt();
        scanner.nextLine();
        return barcode;
    }

    private static String getProductNameFromUser() {
        System.out.print("Ürün adı: ");
        return scanner.nextLine();
    }

    private static int getQuantityFromUser() {
        System.out.print("Miktar: ");
        int quantity = scanner.nextInt();
        scanner.nextLine();
        return quantity;
    }

    private static int getIncreaseAmountFromUser() {
        System.out.print("Artış miktarı: ");
        int amount = scanner.nextInt();
        scanner.nextLine();
        return amount;
    }

    private static int getDecreaseAmountFromUser() {
        System.out.print("Azalış miktarı: ");
        int amount = scanner.nextInt();
        scanner.nextLine();
        return amount;
    }

    // ============ EKRAN MESAJLARI ============

    private static void exitProgram() {
        System.out.println("Programdan çıkılıyor...");
    }

    private static void printInvalidChoice() {
        System.out.println("Geçersiz seçim, tekrar deneyin.");
    }

    private static void printDataLoadedSuccessfully() {
        System.out.println("Başlangıç verisi yüklendi.");
    }

    private static void printFileReadError(IOException error) {
        System.out.println(FILE_NAME + " okunurken hata: " + error.getMessage());
    }

    private static void printProductInsertedOrUpdated() {
        System.out.println("Ürün eklendi / güncellendi.");
    }

    private static void printCurrentStockStatus() {
        System.out.println("Güncel stok durumu (in-order):");
    }
}
