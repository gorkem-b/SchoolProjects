import java.util.ArrayList;
import java.util.List;

public class ProductBST {

    Node root;

    public ProductBST() {
        root = null;
    }

    // ============ EKLEME İŞLEMLERİ ============

    // Yeni ürün ekler veya aynı barkod varsa ürünü günceller.
    public void insert(int barcodeNumber, String productName, int quantity) {
        root = insertRecursive(root, barcodeNumber, productName, quantity);
    }

    private Node insertRecursive(Node current, int barcodeNumber, String productName, int quantity) {
        if (current == null) {
            return new Node(barcodeNumber, productName, quantity);
        }

        if (barcodeNumber < current.barcodeNumber) {
            current.left = insertRecursive(current.left, barcodeNumber, productName, quantity);
            return current;
        }

        if (barcodeNumber > current.barcodeNumber) {
            current.right = insertRecursive(current.right, barcodeNumber, productName, quantity);
            return current;
        }

        // Aynı barkod: ürün bilgilerini güncelle
        updateExistingProduct(current, productName, quantity, barcodeNumber);
        return current;
    }

    private void updateExistingProduct(Node product, String newName, int newQuantity, int barcode) {
        product.productName = newName;
        product.quantity = newQuantity;
        System.out.println("Bu barkod numarasına sahip ürün zaten var, bilgiler güncellendi: " + barcode);
    }

    // ============ ARAMA İŞLEMLERİ ============

    public Node find(int barcodeNumber) {
        return findRecursive(root, barcodeNumber);
    }

    private Node findRecursive(Node current, int barcodeNumber) {
        if (current == null) {
            return null;
        }

        if (barcodeNumber == current.barcodeNumber) {
            return current;
        }

        if (barcodeNumber < current.barcodeNumber) {
            return findRecursive(current.left, barcodeNumber);
        }

        return findRecursive(current.right, barcodeNumber);
    }

    // ============ MİKTAR İŞLEMLERİ ============

    // true dönerse miktar gerçekten değiştirildi (dosyaya kaydetmek için bilgi)
    public boolean increaseQuantity(int barcodeNumber, int amount) {
        Node product = find(barcodeNumber);

        if (product == null) {
            printProductNotFound(barcodeNumber);
            return false;
        }

        product.quantity += amount;
        printQuantityIncreased(product.quantity);
        return true;
    }

    public boolean decreaseQuantity(int barcodeNumber, int amount) {
        Node product = find(barcodeNumber);

        if (product == null) {
            printProductNotFound(barcodeNumber);
            return false;
        }

        int newQuantity = product.quantity - amount;

        if (newQuantity < 0) {
            printCannotGoNegative();
            return false;
        }

        product.quantity = newQuantity;
        printQuantityDecreased(newQuantity);
        return true;
    }

    // ============ YAZDIRMA İŞLEMLERİ ============

    public void printInOrder() {
        printInOrderRecursive(root);
    }

    private void printInOrderRecursive(Node current) {
        if (current == null) {
            return;
        }

        printInOrderRecursive(current.left);
        printProductLine(current);
        printInOrderRecursive(current.right);
    }

    // -------- 7. MADDE: MİKTARA GÖRE ARTAN (SAF BST) --------

    // Sadece 7. madde için: quantity'ye göre sıralayan geçici BST düğümü
    private static class QuantityNode {
        Node product;          // Asıl ürün düğümüne referans
        QuantityNode left;
        QuantityNode right;

        public QuantityNode(Node product) {
            this.product = product;
        }
    }

    // 7. madde: miktara göre artan yazdır (saf BST mantığıyla)
    public void printByQuantityAscending() {
        // 1) Ana BST'deki tüm düğümleri, quantity'ye göre ikinci bir BST'ye yerleştir
        QuantityNode quantityRoot = null;
        quantityRoot = buildQuantityTree(root, quantityRoot);

        // 2) Bu ikinci ağacı in-order dolaşıp yazdır
        System.out.println("Miktara göre artan sırada ürünler:");
        printQuantityTreeInOrder(quantityRoot);
    }

    // Ana barkod-BST'yi dolaşarak quantity-BST'yi kur
    private QuantityNode buildQuantityTree(Node current, QuantityNode qRoot) {
        if (current == null) {
            return qRoot;
        }

        // Sol alt ağacı önce işle
        qRoot = buildQuantityTree(current.left, qRoot);

        // Şimdiki düğümü quantity-BST'ye ekle
        qRoot = insertIntoQuantityTree(qRoot, current);

        // Sağ alt ağacı işle
        qRoot = buildQuantityTree(current.right, qRoot);

        return qRoot;
    }

    // quantity-BST içine insert (anahtar: quantity, eşitlik durumunda barkod ile kır)
    private QuantityNode insertIntoQuantityTree(QuantityNode qRoot, Node product) {
        if (qRoot == null) {
            return new QuantityNode(product);
        }

        int q = product.quantity;
        int rootQ = qRoot.product.quantity;

        if (q < rootQ) {
            qRoot.left = insertIntoQuantityTree(qRoot.left, product);
        } else if (q > rootQ) {
            qRoot.right = insertIntoQuantityTree(qRoot.right, product);
        } else {
            // quantity eşitse, barkoda göre bir tie-breaker kullan
            if (product.barcodeNumber < qRoot.product.barcodeNumber) {
                qRoot.left = insertIntoQuantityTree(qRoot.left, product);
            } else {
                qRoot.right = insertIntoQuantityTree(qRoot.right, product);
            }
        }

        return qRoot;
    }

    // quantity-BST'yi in-order dolaşıp yazdır
    private void printQuantityTreeInOrder(QuantityNode qRoot) {
        if (qRoot == null) {
            return;
        }

        printQuantityTreeInOrder(qRoot.left);

        Node p = qRoot.product;
        System.out.println(
                p.quantity + " | " +
                        p.barcodeNumber + " | " +
                        p.productName
        );

        printQuantityTreeInOrder(qRoot.right);
    }

    // ============ YARDIMCI METOTLAR ============

    // Ağacı in-order dolaşıp barkod sırasına göre liste üretir
    // (inputBst.txt'ye geri yazmak için Main tarafından kullanılabilir).
    public List<Node> toListInOrder() {
        List<Node> productList = new ArrayList<>();
        collectNodes(root, productList);
        return productList;
    }

    private void collectNodes(Node current, List<Node> list) {
        if (current == null) {
            return;
        }

        collectNodes(current.left, list);
        list.add(current);
        collectNodes(current.right, list);
    }

    private void printProductLine(Node product) {
        System.out.println(
                product.barcodeNumber + " | " +
                        product.productName + " | " +
                        product.quantity
        );
    }

    private void printProductNotFound(int barcode) {
        System.out.println("Bu barkod numarasına sahip ürün bulunamadı: " + barcode);
    }

    private void printQuantityIncreased(int newQuantity) {
        System.out.println("Miktar artırıldı. Yeni miktar: " + newQuantity);
    }

    private void printQuantityDecreased(int newQuantity) {
        System.out.println("Miktar azaltıldı. Yeni miktar: " + newQuantity);
    }

    private void printCannotGoNegative() {
        System.out.println("Uyarı: Miktar 0'ın altına düşemez. İşlem yapılmadı.");
    }
}
