using Unity.FPS.Game;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class ObjectiveToast : MonoBehaviour
    {
        [Header("References")] [Tooltip("Text content that will display the title")]
        public TMPro.TextMeshProUGUI TitleTextContent;

        [Tooltip("Text content that will display the description")]
        public TMPro.TextMeshProUGUI DescriptionTextContent;

        [Tooltip("Text content that will display the counter")]
        public TMPro.TextMeshProUGUI CounterTextContent;

        [Tooltip("Rect that will display the description")]
        public RectTransform SubTitleRect;

        [Tooltip("Canvas used to fade in and out the content")]
        public CanvasGroup CanvasGroup;

        [Tooltip("Layout group containing the objective")]
        public HorizontalOrVerticalLayoutGroup LayoutGroup;

        [Header("Transitions")] [Tooltip("Delay before moving complete")]
        public float CompletionDelay;

        [Tooltip("Duration of the fade in")] public float FadeInDuration = 0.5f;
        [Tooltip("Duration of the fade out")] public float FadeOutDuration = 2f;

        [Header("Sound")] [Tooltip("Sound that will be player on initialization")]
        public AudioClip InitSound;

        [Tooltip("Sound that will be player on completion")]
        public AudioClip CompletedSound;

        [Header("Movement")] [Tooltip("Time it takes to move in the screen")]
        public float MoveInDuration = 0.5f;

        [Tooltip("Animation curve for move in, position in x over time")]
        public AnimationCurve MoveInCurve;

        [Tooltip("Time it takes to move out of the screen")]
        public float MoveOutDuration = 2f;

        [Tooltip("Animation curve for move out, position in x over time")]
        public AnimationCurve MoveOutCurve;

        [Header("Update Feedback")]
        [Tooltip("Duration of the emphasis pulse when objective text changes")]
        public float UpdatePulseDuration = 0.35f;

        [Tooltip("How much the toast scales up during objective updates")]
        public float UpdatePulseScaleMultiplier = 1.05f;

        float m_StartFadeTime;
        bool m_IsFadingIn;
        bool m_IsFadingOut;
        bool m_IsMovingIn;
        bool m_IsMovingOut;
        float m_UpdatePulseStartTime = Mathf.NegativeInfinity;
        AudioSource m_AudioSource;
        RectTransform m_RectTransform;
        Vector3 m_BaseScale = Vector3.one;

        void Awake()
        {
            m_RectTransform = GetComponent<RectTransform>();
            m_BaseScale = m_RectTransform != null ? m_RectTransform.localScale : transform.localScale;
        }

        public void Initialize(string titleText, string descText, string counterText, bool isOptionnal, float delay)
        {
            // set the description for the objective, and forces the content size fitter to be recalculated
            Canvas.ForceUpdateCanvases();

            if (m_RectTransform == null)
                m_RectTransform = GetComponent<RectTransform>();
            m_BaseScale = m_RectTransform != null ? m_RectTransform.localScale : transform.localScale;

            TitleTextContent.text = titleText;
            DescriptionTextContent.text = descText;
            CounterTextContent.text = counterText;

            if (GetComponent<RectTransform>())
            {
                LayoutRebuilder.ForceRebuildLayoutImmediate(GetComponent<RectTransform>());
            }

            m_StartFadeTime = Time.unscaledTime + delay;
            // start the fade in
            m_IsFadingIn = true;
            m_IsMovingIn = true;
        }

        public void Complete()
        {
            m_StartFadeTime = Time.unscaledTime + CompletionDelay;
            m_IsFadingIn = false;
            m_IsMovingIn = false;
            m_UpdatePulseStartTime = Mathf.NegativeInfinity;
            ApplyBaseScale();

            // if a sound was set, play it
            PlaySound(CompletedSound);

            // start the fade out
            m_IsFadingOut = true;
            m_IsMovingOut = true;
        }

        void Update()
        {
            float currentTime = Time.unscaledTime;
            float timeSinceFadeStarted = currentTime - m_StartFadeTime;

            SubTitleRect.gameObject.SetActive(!string.IsNullOrEmpty(DescriptionTextContent.text));

            if (m_IsFadingIn && !m_IsFadingOut)
            {
                // fade in
                if (timeSinceFadeStarted < FadeInDuration)
                {
                    // calculate alpha ratio
                    CanvasGroup.alpha = timeSinceFadeStarted / Mathf.Max(0.01f, FadeInDuration);
                }
                else
                {
                    CanvasGroup.alpha = 1f;
                    // end the fade in
                    m_IsFadingIn = false;

                    PlaySound(InitSound);
                }
            }

            if (m_IsMovingIn && !m_IsMovingOut)
            {
                // move in
                if (timeSinceFadeStarted < MoveInDuration)
                {
                    LayoutGroup.padding.left = (int) MoveInCurve.Evaluate(timeSinceFadeStarted / Mathf.Max(0.01f, MoveInDuration));

                    if (GetComponent<RectTransform>())
                    {
                        LayoutRebuilder.ForceRebuildLayoutImmediate(GetComponent<RectTransform>());
                    }
                }
                else
                {
                    // making sure the position is exact
                    LayoutGroup.padding.left = 0;

                    if (GetComponent<RectTransform>())
                    {
                        LayoutRebuilder.ForceRebuildLayoutImmediate(GetComponent<RectTransform>());
                    }

                    m_IsMovingIn = false;
                }

            }

            if (m_IsFadingOut)
            {
                // fade out
                if (timeSinceFadeStarted < FadeOutDuration)
                {
                    // calculate alpha ratio
                    CanvasGroup.alpha = 1 - (timeSinceFadeStarted) / Mathf.Max(0.01f, FadeOutDuration);
                }
                else
                {
                    CanvasGroup.alpha = 0f;

                    // end the fade out, then destroy the object
                    m_IsFadingOut = false;
                    Destroy(gameObject);
                }
            }

            if (m_IsMovingOut)
            {
                // move out
                if (timeSinceFadeStarted < MoveOutDuration)
                {
                    LayoutGroup.padding.left = (int) MoveOutCurve.Evaluate(timeSinceFadeStarted / Mathf.Max(0.01f, MoveOutDuration));

                    if (GetComponent<RectTransform>())
                    {
                        LayoutRebuilder.ForceRebuildLayoutImmediate(GetComponent<RectTransform>());
                    }
                }
                else
                {
                    m_IsMovingOut = false;
                }
            }

            UpdatePulseVisual();
        }

        public void TriggerAttentionPulse()
        {
            m_UpdatePulseStartTime = Time.unscaledTime;
        }

        void UpdatePulseVisual()
        {
            if (UpdatePulseDuration <= 0f)
            {
                ApplyBaseScale();
                return;
            }

            if (Time.unscaledTime < m_UpdatePulseStartTime)
            {
                ApplyBaseScale();
                return;
            }

            float t = (Time.unscaledTime - m_UpdatePulseStartTime) / UpdatePulseDuration;
            if (t >= 1f)
            {
                ApplyBaseScale();
                return;
            }

            float pulse = 1f + Mathf.Sin(t * Mathf.PI) * Mathf.Max(0f, UpdatePulseScaleMultiplier - 1f);
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

        void PlaySound(AudioClip sound)
        {
            if (!sound)
                return;

            if (!m_AudioSource)
            {
                m_AudioSource = gameObject.AddComponent<AudioSource>();
                m_AudioSource.outputAudioMixerGroup = AudioUtility.GetAudioGroup(AudioUtility.AudioGroups.HUDObjective);
            }

            m_AudioSource.PlayOneShot(sound);
        }
    }
}