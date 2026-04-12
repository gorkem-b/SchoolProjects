using TMPro;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public static class PurgeMenuThemeRuntime
    {
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
        static void ApplyTheme()
        {
            Scene scene = SceneManager.GetActiveScene();
            if (scene.name != "IntroMenu" && scene.name != "WinScene")
                return;

            Canvas canvas = Object.FindFirstObjectByType<Canvas>();
            if (canvas == null)
                return;

            EnsureSceneBackgroundVisible(canvas.transform, scene.name);
            EnsureBackdrop(canvas.transform, scene.name);
            if (scene.name == "WinScene")
                NormalizeLayout(canvas.transform, scene.name);
            StyleButtons(canvas.transform);
            StyleTexts(canvas.transform, scene.name);
        }

        static void EnsureSceneBackgroundVisible(Transform root, string sceneName)
        {
            if (sceneName != "WinScene" || root == null)
                return;

            Transform background = root.Find("BackgroundImage");
            if (background != null && !background.gameObject.activeSelf)
                background.gameObject.SetActive(true);
        }

        static void NormalizeLayout(Transform root, string sceneName)
        {
            TextMeshProUGUI title = FindTitle(root);
            if (title != null)
            {
                RectTransform titleRect = title.rectTransform;
                titleRect.anchorMin = new Vector2(0.5f, 0.5f);
                titleRect.anchorMax = new Vector2(0.5f, 0.5f);
                titleRect.pivot = new Vector2(0.5f, 0.5f);
                titleRect.anchoredPosition = Vector2.zero;
                titleRect.sizeDelta = new Vector2(935.65f, 175f);
                title.enableAutoSizing = false;
                title.fontSize = 72f;
                title.alignment = TextAlignmentOptions.Center;
            }

            Button[] buttons = root.GetComponentsInChildren<Button>(true);
            System.Array.Sort(buttons, (left, right) =>
            {
                float leftY = left != null ? left.GetComponent<RectTransform>().anchoredPosition.y : 0f;
                float rightY = right != null ? right.GetComponent<RectTransform>().anchoredPosition.y : 0f;
                return leftY.CompareTo(rightY);
            });

            if (buttons.Length > 0)
                ApplyButtonLayout(buttons[0], 25f, 70f, true, sceneName == "WinScene" ? "PLAY AGAIN" : "PLAY");

            if (buttons.Length > 1)
                ApplyButtonLayout(buttons[1], 105f, 30f, false, sceneName == "WinScene" ? "MENU" : "CONTROLS");
        }

        static TextMeshProUGUI FindTitle(Transform root)
        {
            TextMeshProUGUI[] texts = root.GetComponentsInChildren<TextMeshProUGUI>(true);
            TextMeshProUGUI title = null;
            for (int i = 0; i < texts.Length; i++)
            {
                if (texts[i].transform.GetComponentInParent<Button>() != null)
                    continue;

                if (title == null || texts[i].fontSize > title.fontSize)
                    title = texts[i];
            }

            return title;
        }

        static void ApplyButtonLayout(Button button, float anchoredY, float height, bool primary, string labelText)
        {
            if (button == null)
                return;

            RectTransform rect = button.GetComponent<RectTransform>();
            if (rect != null)
            {
                rect.anchorMin = new Vector2(0.5f, 0f);
                rect.anchorMax = new Vector2(0.5f, 0f);
                rect.pivot = new Vector2(0.5f, 0f);
                rect.anchoredPosition = new Vector2(0f, anchoredY);
                rect.sizeDelta = new Vector2(380f, height);
            }

            TextMeshProUGUI label = button.GetComponentInChildren<TextMeshProUGUI>(true);
            if (label != null)
            {
                label.text = labelText;
                label.fontStyle = FontStyles.UpperCase;
                label.enableAutoSizing = true;
                label.fontSizeMin = 12f;
                label.fontSizeMax = primary ? 45f : 25f;
                label.alignment = TextAlignmentOptions.Center;
            }
        }

        static void EnsureBackdrop(Transform canvasTransform, string sceneName)
        {
            if (canvasTransform.Find("PurgeThemeBackdrop") != null)
                return;

            GameObject backdrop = new GameObject("PurgeThemeBackdrop", typeof(RectTransform), typeof(Image));
            backdrop.transform.SetParent(canvasTransform, false);
            backdrop.transform.SetSiblingIndex(0);

            RectTransform rect = backdrop.GetComponent<RectTransform>();
            rect.anchorMin = Vector2.zero;
            rect.anchorMax = Vector2.one;
            rect.offsetMin = Vector2.zero;
            rect.offsetMax = Vector2.zero;

            Image image = backdrop.GetComponent<Image>();
            image.color = new Color(0.03f, 0.05f, 0.09f, 1f);

            CreateAccentBar(backdrop.transform, "TopAccent", new Vector2(0.5f, 1f), new Vector2(0.5f, 1f), new Vector2(0f, -24f), new Vector2(0f, 14f), new Color(0.12f, 0.78f, 0.92f, 0.7f));
            CreateAccentBar(backdrop.transform, "BottomAccent", new Vector2(0.5f, 0f), new Vector2(0.5f, 0f), new Vector2(0f, 24f), new Vector2(0f, 10f), new Color(0.05f, 0.55f, 0.72f, 0.45f));
        }

        static void CreateAccentBar(Transform parent, string name, Vector2 anchorMin, Vector2 anchorMax, Vector2 anchoredPosition, Vector2 sizeDelta, Color color)
        {
            GameObject bar = new GameObject(name, typeof(RectTransform), typeof(Image));
            bar.transform.SetParent(parent, false);
            RectTransform rect = bar.GetComponent<RectTransform>();
            rect.anchorMin = anchorMin;
            rect.anchorMax = anchorMax;
            rect.pivot = new Vector2(0.5f, 0.5f);
            rect.anchoredPosition = anchoredPosition;
            rect.sizeDelta = sizeDelta;
            bar.GetComponent<Image>().color = color;
        }

        static void StyleButtons(Transform root)
        {
            Button[] buttons = root.GetComponentsInChildren<Button>(true);
            for (int i = 0; i < buttons.Length; i++)
            {
                Image image = buttons[i].GetComponent<Image>();
                if (image != null)
                    image.color = new Color(0.08f, 0.18f, 0.24f, 0.92f);

                TextMeshProUGUI label = buttons[i].GetComponentInChildren<TextMeshProUGUI>(true);
                if (label != null)
                    label.color = new Color(0.65f, 0.93f, 1f, 1f);
            }
        }

        static void StyleTexts(Transform root, string sceneName)
        {
            TextMeshProUGUI[] texts = root.GetComponentsInChildren<TextMeshProUGUI>(true);
            TextMeshProUGUI title = FindTitle(root);
            for (int i = 0; i < texts.Length; i++)
            {
                if (texts[i].transform.GetComponentInParent<Button>() == null)
                    texts[i].color = new Color(0.72f, 0.94f, 1f, 1f);
            }

            if (title != null)
            {
                title.text = sceneName == "WinScene" ? "PURGE COMPLETE" : "PURGE";
                title.color = new Color(0.58f, 0.96f, 1f, 1f);
            }
        }
    }
}