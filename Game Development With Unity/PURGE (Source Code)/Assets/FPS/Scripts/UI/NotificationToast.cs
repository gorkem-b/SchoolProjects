using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class NotificationToast : MonoBehaviour
    {
        [Tooltip("Text content that will display the notification text")]
        public TMPro.TextMeshProUGUI TextContent;
        [Tooltip("Canvas used to fade in and out the content")]
        public CanvasGroup CanvasGroup;
        [Tooltip("Optional background image used to style the toast")]
        public Image BackgroundImage;
        [Tooltip("How long it will stay visible")]
        public float VisibleDuration;
        [Tooltip("Duration of the fade in")]
        public float FadeInDuration = 0.5f;
        [Tooltip("Duration of the fade out")]
        public float FadeOutDuration = 2f;

        [Header("Refresh Feedback")]
        [Tooltip("Extra visible time added when the same toast is refreshed")]
        public float RefreshVisibleDurationBoost = 0.6f;

        [Tooltip("Duration of the scale pulse when the toast is refreshed")]
        public float RefreshPulseDuration = 0.28f;

        [Tooltip("Scale multiplier reached during refresh pulse")]
        public float RefreshPulseScaleMultiplier = 1.06f;

        [Header("Narrative Subtitle Style")]
        public Vector2 NarrativeSize = new Vector2(700f, 36f);
        public Vector2 NarrativeAnchoredPosition = new Vector2(0f, 8f);
        public float NarrativeVisibleDuration = 4.2f;
        public float NarrativeFadeInDuration = 0.15f;
        public float NarrativeFadeOutDuration = 0.6f;
        public float NarrativeFontSize = 14f;
        public FontStyles NarrativeFontStyle = FontStyles.Normal;
        public TextAlignmentOptions NarrativeAlignment = TextAlignmentOptions.Center;
        public Color NarrativeTextColor = new Color(0.94f, 0.96f, 0.98f, 1f);
        public Color NarrativeBackgroundColor = new Color(0f, 0f, 0f, 0f);

        public bool Initialized { get; private set; }
        public bool UsesNarrativeStyle { get; private set; }
        float m_InitTime;
        float m_VisibleUntilTime;
        float m_RefreshPulseStartTime = Mathf.NegativeInfinity;
        RectTransform m_RectTransform;
        Vector3 m_BaseScale = Vector3.one;
        Vector2 m_DefaultAnchorMin;
        Vector2 m_DefaultAnchorMax;
        Vector2 m_DefaultPivot;
        Vector2 m_DefaultAnchoredPosition;
        Vector2 m_DefaultSizeDelta;
        float m_DefaultVisibleDuration;
        float m_DefaultFadeInDuration;
        float m_DefaultFadeOutDuration;
        float m_DefaultFontSize;
        FontStyles m_DefaultFontStyle;
        TextAlignmentOptions m_DefaultAlignment;
        Color m_DefaultTextColor = Color.white;
        Color m_DefaultBackgroundColor = Color.white;
        bool m_DefaultBackgroundEnabled = true;

        public float TotalRunTime => VisibleDuration + FadeInDuration + FadeOutDuration;

        void Awake()
        {
            m_RectTransform = GetComponent<RectTransform>();
            m_BaseScale = m_RectTransform != null ? m_RectTransform.localScale : transform.localScale;

            if (BackgroundImage == null)
                BackgroundImage = GetComponentInChildren<Image>(true);

            if (m_RectTransform != null)
            {
                m_DefaultAnchorMin = m_RectTransform.anchorMin;
                m_DefaultAnchorMax = m_RectTransform.anchorMax;
                m_DefaultPivot = m_RectTransform.pivot;
                m_DefaultAnchoredPosition = m_RectTransform.anchoredPosition;
                m_DefaultSizeDelta = m_RectTransform.sizeDelta;
            }

            m_DefaultVisibleDuration = VisibleDuration;
            m_DefaultFadeInDuration = FadeInDuration;
            m_DefaultFadeOutDuration = FadeOutDuration;

            if (TextContent != null)
            {
                m_DefaultFontSize = TextContent.fontSize;
                m_DefaultFontStyle = TextContent.fontStyle;
                m_DefaultAlignment = TextContent.alignment;
                m_DefaultTextColor = TextContent.color;
            }

            if (BackgroundImage != null)
            {
                m_DefaultBackgroundColor = BackgroundImage.color;
                m_DefaultBackgroundEnabled = BackgroundImage.enabled;
            }
        }

        public void Initialize(string text, bool useNarrativeStyle = false)
        {
            UsesNarrativeStyle = useNarrativeStyle;
            ApplyVisualStyle(useNarrativeStyle);
            TextContent.text = text;
            m_InitTime = Time.unscaledTime;
            m_VisibleUntilTime = m_InitTime + FadeInDuration + VisibleDuration;
            m_RefreshPulseStartTime = Mathf.NegativeInfinity;
            ApplyBaseScale();

            // start the fade out
            Initialized = true;
        }

        public bool MatchesText(string text)
        {
            return Initialized && TextContent != null && TextContent.text == text;
        }

        public void ExtendAndPulse(float extraVisibleTime = -1f)
        {
            if (!Initialized)
                return;

            float safeExtraTime = extraVisibleTime > 0f ? extraVisibleTime : RefreshVisibleDurationBoost;
            m_VisibleUntilTime = Mathf.Max(m_VisibleUntilTime, Time.unscaledTime) + safeExtraTime;
            m_RefreshPulseStartTime = Time.unscaledTime;
        }

        void Update()
        {
            if (Initialized)
            {
                float currentTime = Time.unscaledTime;
                float timeSinceInit = currentTime - m_InitTime;
                if (timeSinceInit < FadeInDuration)
                {
                    // fade in
                    CanvasGroup.alpha = timeSinceInit / FadeInDuration;
                }
                else if (currentTime < m_VisibleUntilTime)
                {
                    // stay visible
                    CanvasGroup.alpha = 1f;
                }
                else if (currentTime < m_VisibleUntilTime + FadeOutDuration)
                {
                    // fade out
                    CanvasGroup.alpha = 1 - (currentTime - m_VisibleUntilTime) / FadeOutDuration;
                }
                else
                {
                    CanvasGroup.alpha = 0f;

                    // fade out over, destroy the object
                    Destroy(gameObject);
                }

                UpdatePulseVisual();
            }
        }

        void UpdatePulseVisual()
        {
            float currentTime = Time.unscaledTime;

            if (RefreshPulseDuration <= 0f)
            {
                ApplyBaseScale();
                return;
            }

            if (currentTime < m_RefreshPulseStartTime)
            {
                ApplyBaseScale();
                return;
            }

            float t = (currentTime - m_RefreshPulseStartTime) / RefreshPulseDuration;
            if (t >= 1f)
            {
                ApplyBaseScale();
                return;
            }

            float pulse = 1f + Mathf.Sin(t * Mathf.PI) * Mathf.Max(0f, RefreshPulseScaleMultiplier - 1f);
            Vector3 scale = m_BaseScale * pulse;
            if (m_RectTransform != null)
                m_RectTransform.localScale = scale;
            else
                transform.localScale = scale;
        }

        void ApplyBaseScale()
        {
            if (m_RectTransform != null)
                m_RectTransform.localScale = m_BaseScale;
            else
                transform.localScale = m_BaseScale;
        }

        void ApplyVisualStyle(bool useNarrativeStyle)
        {
            if (TextContent != null)
            {
                TextContent.fontSize = useNarrativeStyle ? NarrativeFontSize : m_DefaultFontSize;
                TextContent.fontStyle = useNarrativeStyle ? NarrativeFontStyle : m_DefaultFontStyle;
                TextContent.alignment = useNarrativeStyle ? NarrativeAlignment : m_DefaultAlignment;
                TextContent.color = useNarrativeStyle ? NarrativeTextColor : m_DefaultTextColor;
            }

            if (BackgroundImage != null)
            {
                BackgroundImage.enabled = useNarrativeStyle ? false : m_DefaultBackgroundEnabled;
                BackgroundImage.color = useNarrativeStyle ? NarrativeBackgroundColor : m_DefaultBackgroundColor;
            }

            VisibleDuration = useNarrativeStyle ? NarrativeVisibleDuration : m_DefaultVisibleDuration;
            FadeInDuration = useNarrativeStyle ? NarrativeFadeInDuration : m_DefaultFadeInDuration;
            FadeOutDuration = useNarrativeStyle ? NarrativeFadeOutDuration : m_DefaultFadeOutDuration;
        }
    }
}