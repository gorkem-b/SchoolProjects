using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class InGameMenuManager : MonoBehaviour
    {
        [Tooltip("Root GameObject of the menu used to toggle its activation")]
        public GameObject MenuRoot;

        [Tooltip("Ignores pause/menu toggle input for a short time after scene start")]
        public float MenuInputCooldownOnStart = 0.35f;

        [Tooltip("Master volume when menu is open")] [Range(0.001f, 1f)]
        public float VolumeWhenMenuOpen = 0.5f;

        [Tooltip("Slider component for look sensitivity")]
        public Slider LookSensitivitySlider;

        [Tooltip("Slider component for master volume")]
        public Slider MasterVolumeSlider;

        [Tooltip("Toggle component for shadows")]
        public Toggle ShadowsToggle;

        [Tooltip("Toggle component for framerate display")]
        public Toggle FramerateToggle;

        [Tooltip("GameObject for the controls")]
        public GameObject ControlImage;

        PlayerInputHandler m_PlayerInputsHandler;
        FramerateCounter m_FramerateCounter;
        GameFlowManager m_GameFlowManager;
        
        private InputAction m_SubmitAction;
        private InputAction m_CancelAction;
        private InputAction m_NavigateAction;
        private InputAction m_MenuAction;
        float m_MenuInputIgnoreUntil;
        float m_MasterVolume = 1f;

        void Start()
        {
            m_PlayerInputsHandler = FindFirstObjectByType<PlayerInputHandler>();
            DebugUtility.HandleErrorIfNullFindObject<PlayerInputHandler, InGameMenuManager>(m_PlayerInputsHandler,
                this);

            m_FramerateCounter = FindFirstObjectByType<FramerateCounter>();
            DebugUtility.HandleErrorIfNullFindObject<FramerateCounter, InGameMenuManager>(m_FramerateCounter, this);

            m_GameFlowManager = FindFirstObjectByType<GameFlowManager>();

            MenuRoot.SetActive(false);
            Time.timeScale = 1f;
            Cursor.lockState = CursorLockMode.Locked;
            Cursor.visible = false;
            if (ControlImage != null)
            {
                ControlImage.SetActive(false);
            }

            m_MenuInputIgnoreUntil = Time.unscaledTime + Mathf.Max(0f, MenuInputCooldownOnStart);
            m_MasterVolume = Mathf.Clamp(AudioUtility.GetMasterVolume(), 0.001f, 1f);

            if (LookSensitivitySlider != null)
            {
                LookSensitivitySlider.value = m_PlayerInputsHandler.LookSensitivity;
                LookSensitivitySlider.onValueChanged.AddListener(OnMouseSensitivityChanged);
            }

            EnsureMasterVolumeSlider();
            if (MasterVolumeSlider != null)
            {
                MasterVolumeSlider.minValue = 0.05f;
                MasterVolumeSlider.maxValue = 1f;
                MasterVolumeSlider.value = m_MasterVolume;
                MasterVolumeSlider.onValueChanged.AddListener(OnMasterVolumeChanged);
            }

            // Graphics/shadow options are intentionally disabled for PURGE
            if (ShadowsToggle != null)
            {
                ShadowsToggle.transform.parent.gameObject.SetActive(false);
            }

            HideUnsupportedOptions();
            FixMenuNavigation();

            if (FramerateToggle != null && m_FramerateCounter != null && m_FramerateCounter.UIText != null)
            {
                FramerateToggle.isOn = m_FramerateCounter.UIText.gameObject.activeSelf;
                FramerateToggle.onValueChanged.AddListener(OnFramerateCounterChanged);
            }

            m_SubmitAction = InputSystem.actions.FindAction("UI/Submit");
            m_CancelAction = InputSystem.actions.FindAction("UI/Cancel");
            m_NavigateAction = InputSystem.actions.FindAction("UI/Navigate");
            m_MenuAction = InputSystem.actions.FindAction("UI/Menu");
            
            m_SubmitAction.Enable();
            m_CancelAction.Enable();
            m_NavigateAction.Enable();
            m_MenuAction.Enable();
        }

        void Update()
        {
            bool ignoreMenuToggleInput = Time.unscaledTime < m_MenuInputIgnoreUntil;
            bool gameIsEnding = m_GameFlowManager != null && m_GameFlowManager.GameIsEnding;

            if (gameIsEnding)
            {
                if (MenuRoot.activeSelf)
                    SetPauseMenuActivation(false);
                return;
            }

            // Lock cursor when clicking outside of menu
            if (!MenuRoot.activeSelf && Mouse.current.leftButton.wasPressedThisFrame)
            {
                Cursor.lockState = CursorLockMode.Locked;
                Cursor.visible = false;
            }

            if (!ignoreMenuToggleInput && Keyboard.current.escapeKey.wasPressedThisFrame)
            {
                Cursor.lockState = CursorLockMode.None;
                Cursor.visible = true;
            }

            if (!ignoreMenuToggleInput &&
                (m_MenuAction.WasPressedThisFrame()
                || (MenuRoot.activeSelf && m_CancelAction.WasPressedThisFrame())))
            {
                if (ControlImage != null && ControlImage.activeSelf)
                {
                    ControlImage.SetActive(false);
                    return;
                }

                SetPauseMenuActivation(!MenuRoot.activeSelf);

            }

            if (m_NavigateAction.ReadValue<Vector2>().y != 0)
            {
                if (EventSystem.current.currentSelectedGameObject == null)
                {
                    EventSystem.current.SetSelectedGameObject(null);
                    if (LookSensitivitySlider != null)
                        LookSensitivitySlider.Select();
                }
            }
        }

        public void ClosePauseMenu()
        {
            SetPauseMenuActivation(false);
        }

        void SetPauseMenuActivation(bool active)
        {
            MenuRoot.SetActive(active);

            if (MenuRoot.activeSelf)
            {
                Cursor.lockState = CursorLockMode.None;
                Cursor.visible = true;
                Time.timeScale = 0f;
                ApplyConfiguredMasterVolume(true);

                EventSystem.current.SetSelectedGameObject(null);
                if (LookSensitivitySlider != null)
                    LookSensitivitySlider.Select();
            }
            else
            {
                Cursor.lockState = CursorLockMode.Locked;
                Cursor.visible = false;
                Time.timeScale = 1f;
                ApplyConfiguredMasterVolume(false);
            }

        }

        void OnMouseSensitivityChanged(float newValue)
        {
            m_PlayerInputsHandler.LookSensitivity = newValue;
        }

        void OnMasterVolumeChanged(float newValue)
        {
            m_MasterVolume = Mathf.Clamp(newValue, 0.001f, 1f);
            ApplyConfiguredMasterVolume(MenuRoot != null && MenuRoot.activeSelf);
        }

        void HideUnsupportedOptions()
        {
            if (MenuRoot == null)
                return;

            Transform invincibilityPanel = MenuRoot.transform.Find("MenuPanel/InnerPanel/InvincibilityPanel");
            if (invincibilityPanel != null)
                invincibilityPanel.gameObject.SetActive(false);
        }

        void FixMenuNavigation()
        {
            if (FramerateToggle == null)
                return;

            Navigation navigation = FramerateToggle.navigation;
            navigation.selectOnUp = LookSensitivitySlider;
            FramerateToggle.navigation = navigation;
        }

        void OnFramerateCounterChanged(bool newValue)
        {
            m_FramerateCounter.UIText.gameObject.SetActive(newValue);
        }

        public void OnShowControlButtonClicked(bool show)
        {
            if (ControlImage != null)
                ControlImage.SetActive(show);
        }

        void EnsureMasterVolumeSlider()
        {
            if (MasterVolumeSlider != null || LookSensitivitySlider == null)
                return;

            Transform sourcePanel = LookSensitivitySlider.transform.parent;
            if (sourcePanel == null || sourcePanel.parent == null)
                return;

            GameObject volumePanel = Instantiate(sourcePanel.gameObject, sourcePanel.parent);
            volumePanel.name = "MasterVolumePanel";
            volumePanel.transform.SetSiblingIndex(sourcePanel.GetSiblingIndex() + 1);

            TextMeshProUGUI label = volumePanel.GetComponentInChildren<TextMeshProUGUI>(true);
            if (label != null)
                label.text = "Master Volume";

            MasterVolumeSlider = volumePanel.GetComponentInChildren<Slider>(true);
        }

        void ApplyConfiguredMasterVolume(bool menuOpen)
        {
            float appliedVolume = menuOpen ? m_MasterVolume * VolumeWhenMenuOpen : m_MasterVolume;
            AudioUtility.SetMasterVolume(Mathf.Clamp(appliedVolume, 0.001f, 1f));
        }

        void OnDestroy()
        {
            if (LookSensitivitySlider != null)
                LookSensitivitySlider.onValueChanged.RemoveListener(OnMouseSensitivityChanged);
            if (MasterVolumeSlider != null)
                MasterVolumeSlider.onValueChanged.RemoveListener(OnMasterVolumeChanged);
            if (FramerateToggle != null)
                FramerateToggle.onValueChanged.RemoveListener(OnFramerateCounterChanged);

            m_SubmitAction?.Disable();
            m_CancelAction?.Disable();
            m_NavigateAction?.Disable();
            m_MenuAction?.Disable();

            if (MenuRoot != null && MenuRoot.activeSelf)
                SetPauseMenuActivation(false);
        }
    }
}