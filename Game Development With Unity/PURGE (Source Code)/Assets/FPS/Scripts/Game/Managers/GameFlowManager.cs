using UnityEngine;
using UnityEngine.SceneManagement;

namespace Unity.FPS.Game
{
    public class GameFlowManager : MonoBehaviour
    {
        const string NarrativePrefix = "[NARRATIVE]";

        struct NarrativeCue
        {
            public float Delay;
            public string Message;

            public NarrativeCue(float delay, string message)
            {
                Delay = delay;
                Message = message;
            }
        }

        [Header("Parameters")] [Tooltip("Duration of the fade-to-black at the end of the game")]
        public float EndSceneLoadDelay = 3f;

        [Tooltip("The canvas group of the fade-to-black screen")]
        public CanvasGroup EndGameFadeCanvasGroup;

        [Header("Win")] [Tooltip("This string has to be the name of the scene you want to load when winning")]
        public string WinSceneName = "WinScene";

        [Tooltip("Duration of delay before the fade-to-black, if winning")]
        public float DelayBeforeFadeToBlack = 4f;

        [Tooltip("Win game message")]
        public string WinGameMessage;
        [Tooltip("Duration of delay before the win message")]
        public float DelayBeforeWinMessage = 2f;

        [Tooltip("Sound played on win")] public AudioClip VictorySound;

        [Header("Lose")] [Tooltip("This string has to be the name of the scene you want to load when losing")]
        public string LoseSceneName = "LoseScene";

        [Tooltip("Duration of delay before the fade-to-black, if losing")]
        public float DelayBeforeFadeToBlackOnLose = 1.2f;

        [Tooltip("Lose game message")]
        public string LoseGameMessage = "Mission failed";

        [Tooltip("Duration of delay before the lose message")]
        public float DelayBeforeLoseMessage = 0.15f;

        [Tooltip("Sound played on lose")]
        public AudioClip DefeatSound;

        [Header("Narrative Follow-Ups")]
        [Tooltip("Delay before the mid-mission narrative follow-up line")]
        public float MidMissionNarrativeDelay = 24f;

        [Tooltip("Delay before the final approach narrative line")]
        public float FinalMissionNarrativeDelay = 48f;


        public bool GameIsEnding { get; private set; }

        float m_TimeBeginFade;
        float m_TimeLoadEndGameScene;
        string m_SceneToLoad;
        Coroutine m_SceneNarrativeRoutine;

        void Awake()
        {
            EventManager.AddListener<AllObjectivesCompletedEvent>(OnAllObjectivesCompleted);
            EventManager.AddListener<PlayerDeathEvent>(OnPlayerDeath);
        }

        void Start()
        {
            AudioUtility.SetMasterVolume(1);
            ApplyPurgeSceneDefaults();
            StartSceneNarrativeSequence();
        }

        void Update()
        {
            if (GameIsEnding)
            {
                float timeRatio = 0f;
                if (Time.time >= m_TimeBeginFade)
                {
                    timeRatio = Mathf.Clamp01((Time.time - m_TimeBeginFade) / Mathf.Max(0.01f, EndSceneLoadDelay));
                }

                EndGameFadeCanvasGroup.alpha = timeRatio;

                AudioUtility.SetMasterVolume(1 - timeRatio);

                // See if it's time to load the end scene (after the delay)
                if (Time.time >= m_TimeLoadEndGameScene)
                {
                    SceneManager.LoadScene(m_SceneToLoad);
                    GameIsEnding = false;
                }
            }
        }

        void OnAllObjectivesCompleted(AllObjectivesCompletedEvent evt) => EndGame(true);
        void OnPlayerDeath(PlayerDeathEvent evt) => EndGame(false);

        void EndGame(bool win)
        {
            if (GameIsEnding)
                return;

            // unlocks the cursor before leaving the scene, to be able to click buttons
            Cursor.lockState = CursorLockMode.None;
            Cursor.visible = true;

            // Remember that we need to load the appropriate end scene after a delay
            GameIsEnding = true;
            EndGameFadeCanvasGroup.gameObject.SetActive(true);
            EndGameFadeCanvasGroup.alpha = 0f;

            GameOverEvent gameOverEvent = Events.GameOverEvent;
            gameOverEvent.Win = win;
            EventManager.Broadcast(gameOverEvent);

            if (win)
            {
                m_SceneToLoad = WinSceneName;
                m_TimeBeginFade = Time.time + DelayBeforeFadeToBlack;
                m_TimeLoadEndGameScene = m_TimeBeginFade + EndSceneLoadDelay;

                // play a sound on win
                if (VictorySound != null)
                {
                    var audioSource = gameObject.AddComponent<AudioSource>();
                    audioSource.clip = VictorySound;
                    audioSource.playOnAwake = false;
                    audioSource.outputAudioMixerGroup = AudioUtility.GetAudioGroup(AudioUtility.AudioGroups.HUDVictory);
                    audioSource.PlayScheduled(AudioSettings.dspTime + DelayBeforeWinMessage);
                }

                BroadcastEndGameMessage(WinGameMessage, DelayBeforeWinMessage);
            }
            else
            {
                // PURGE flow: on death restart the current level instead of loading a dedicated lose scene
                Scene activeScene = SceneManager.GetActiveScene();
                m_SceneToLoad = activeScene.IsValid() ? activeScene.name : LoseSceneName;
                m_TimeBeginFade = Time.time + DelayBeforeFadeToBlackOnLose;
                m_TimeLoadEndGameScene = m_TimeBeginFade + EndSceneLoadDelay;

                if (DefeatSound != null)
                {
                    var audioSource = gameObject.AddComponent<AudioSource>();
                    audioSource.clip = DefeatSound;
                    audioSource.playOnAwake = false;
                    audioSource.outputAudioMixerGroup = AudioUtility.GetAudioGroup(AudioUtility.AudioGroups.HUDVictory);
                    audioSource.PlayScheduled(AudioSettings.dspTime + DelayBeforeLoseMessage);
                }

                BroadcastEndGameMessage(LoseGameMessage, DelayBeforeLoseMessage);
            }
        }

        void BroadcastEndGameMessage(string message, float delay)
        {
            if (string.IsNullOrEmpty(message))
                return;

            DisplayMessageEvent displayMessage = Events.DisplayMessageEvent;
            displayMessage.Message = message;
            displayMessage.DelayBeforeDisplay = delay;
            EventManager.Broadcast(displayMessage);
        }

        void ApplyPurgeSceneDefaults()
        {
            Scene activeScene = SceneManager.GetActiveScene();
            if (!activeScene.IsValid())
                return;

            switch (activeScene.name)
            {
                case "HangarScene":
                    WinSceneName = "CorridorScene";
                    WinGameMessage = "Level 1 complete";
                    break;

                case "CorridorScene":
                    WinSceneName = "CoreScene";
                    WinGameMessage = "Level 2 complete";
                    break;

                case "CoreScene":
                    WinSceneName = "CoolingDepotsScene";
                    WinGameMessage = "Level 3 complete";
                    break;

                case "CoolingDepotsScene":
                    WinSceneName = "ReactorCavernScene";
                    WinGameMessage = "Level 4 complete";
                    break;

                case "ReactorCavernScene":
                    WinSceneName = "WinScene";
                    WinGameMessage = "Purge complete";
                    break;

                default:
                    return;
            }

            LoseSceneName = "LoseScene";
        }

        void StartSceneNarrativeSequence()
        {
            NarrativeCue[] cues = GetSceneNarrativeCues();
            if (cues == null || cues.Length == 0)
                return;

            if (m_SceneNarrativeRoutine != null)
                StopCoroutine(m_SceneNarrativeRoutine);

            m_SceneNarrativeRoutine = StartCoroutine(PlaySceneNarrativeSequence(cues));
        }

        System.Collections.IEnumerator PlaySceneNarrativeSequence(NarrativeCue[] cues)
        {
            float previousDelay = 0f;
            for (int i = 0; i < cues.Length; i++)
            {
                NarrativeCue cue = cues[i];
                float waitDuration = Mathf.Max(0f, cue.Delay - previousDelay);
                previousDelay = cue.Delay;

                if (waitDuration > 0f)
                    yield return new WaitForSecondsRealtime(waitDuration);

                BroadcastSceneBriefing(cue.Message);
            }

            m_SceneNarrativeRoutine = null;
        }

        NarrativeCue[] GetSceneNarrativeCues()
        {
            Scene activeScene = SceneManager.GetActiveScene();
            if (!activeScene.IsValid())
                return null;

            switch (activeScene.name)
            {
                case "HangarScene":
                    return BuildNarrativeSequence(
                        "Sector 4 was sealed on the night the evacuation failed. The manifests were never closed.",
                        "Cargo lifts stopped mid-cycle, leaving half the station's supplies stranded in this bay.",
                        "Security locked the hangar from the inside after something rose from the lower decks.",
                        "Burn marks along the floor still point toward the corridor routes the survivors tried to reach.",
                        "Those corridors lead into the buried spine of the facility, where the first blackout was erased from record.");

                case "CorridorScene":
                    return BuildNarrativeSequence(
                        "These armored halls were built to protect the administrative wing if the outer sectors collapsed.",
                        "When the lockdown began, checkpoint doors sealed one after another until the passage became a trap.",
                        "The turrets still follow orders from officers who died before the alarms ever stopped.",
                        "Backup power kept the quarantine network alive below while the upper station was left to die.",
                        "The routes ahead descend toward the silo that fed the hidden machinery under the complex.");

                case "CoreScene":
                    return BuildNarrativeSequence(
                        "The central silo never appeared on civilian plans. It was wrapped around systems no outsider should see.",
                        "Maintenance records end the same night the quarantine grid came alive and the lower floors vanished from reports.",
                        "Power is still flowing downward, holding ancient locks and filtration systems between failure and purpose.",
                        "The deeper levels feel less like engineering and more like a vault built around a secret.",
                        "Everything here feeds the cooling depots, where the station's buried heart is still being kept alive.");

                case "CoolingDepotsScene":
                    return BuildNarrativeSequence(
                        "The cooling depots were the last barrier between the reactor and a chain failure that would have split the mountain open.",
                        "Pumps still labor through the pipes, though no crew remains to watch the pressure rising in the dark.",
                        "Frost covers scorch marks older than the purge itself, as if this place has already survived one disaster.",
                        "The drone patrols drift through the lanes like custodians preserving a scene no one was meant to revisit.",
                        "Beyond the depots, the rock opens into the chamber where the reactor and the truth were sealed together.");

                case "ReactorCavernScene":
                    return BuildNarrativeSequence(
                        "The reactor cavern was buried beneath false walls and sealed lifts because this chamber was never meant to be found again.",
                        "The mech is the last machine still obeying the station's final command: deny all access to the core.",
                        "The stone around it still bears the scars of the first containment attempt, when the facility failed to seal its wound.",
                        "Every alarm in the complex seems to end here, as if the entire station existed to keep this chamber hidden.",
                        "When the mech falls, nothing will remain between you and what the purge was trying to erase.");

                default:
                    return null;
            }
        }

        NarrativeCue[] BuildNarrativeSequence(params string[] messages)
        {
            if (messages == null || messages.Length == 0)
                return null;

            float earlyDelay = Mathf.Max(10f, MidMissionNarrativeDelay * 0.5f);
            float midDelay = Mathf.Max(earlyDelay + 8f, MidMissionNarrativeDelay);
            float lateDelay = Mathf.Max(midDelay + 8f, Mathf.Lerp(MidMissionNarrativeDelay, FinalMissionNarrativeDelay, 0.55f));
            float finalDelay = Mathf.Max(lateDelay + 8f, FinalMissionNarrativeDelay);

            float[] defaultDelays =
            {
                0.15f,
                earlyDelay,
                midDelay,
                lateDelay,
                finalDelay,
                finalDelay + 10f,
            };

            NarrativeCue[] cues = new NarrativeCue[messages.Length];
            for (int i = 0; i < messages.Length; i++)
            {
                float delay = i < defaultDelays.Length
                    ? defaultDelays[i]
                    : defaultDelays[defaultDelays.Length - 1] + 10f * (i - defaultDelays.Length + 1);

                cues[i] = new NarrativeCue(delay, messages[i]);
            }

            return cues;
        }

        void BroadcastSceneBriefing(string message)
        {
            if (string.IsNullOrWhiteSpace(message))
                return;

            DisplayMessageEvent displayMessage = Events.DisplayMessageEvent;
            displayMessage.Message = NarrativePrefix + " " + message;
            displayMessage.DelayBeforeDisplay = 0.15f;
            EventManager.Broadcast(displayMessage);
        }

        void OnDestroy()
        {
            if (m_SceneNarrativeRoutine != null)
                StopCoroutine(m_SceneNarrativeRoutine);

            EventManager.RemoveListener<AllObjectivesCompletedEvent>(OnAllObjectivesCompleted);
            EventManager.RemoveListener<PlayerDeathEvent>(OnPlayerDeath);
        }
    }
}