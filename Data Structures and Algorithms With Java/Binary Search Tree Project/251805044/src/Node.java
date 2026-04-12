public class Node {
    int barcodeNumber;
    String productName;
    int quantity;

    Node left;
    Node right;

    public Node(int barcodeNumber, String productName, int quantity) {
        this.barcodeNumber = barcodeNumber;
        this.productName = productName;
        this.quantity = quantity;
        this.left = null;
        this.right = null;
    }
}
