using UnityEngine;
using UnityEngine.Audio;
using UnityEngine.SceneManagement;

namespace Unity.FPS.Game
{
    [DisallowMultipleComponent]
    public class PurgeMusicManager : MonoBehaviour
    {
        const string MenuResourcesPath = "PurgeMusic/Menu";
        const string GameplayResourcesPath = "PurgeMusic/Gameplay";
        const string BossResourcesPath = "PurgeMusic/Boss";
        const string WinResourcesPath = "PurgeMusic/Win";
        const string LoseResourcesPath = "PurgeMusic/Lose";

        public AudioClip[] MenuTracks;
        public AudioClip[] GameplayTracks;
        public AudioClip[] BossTracks;
        public AudioClip[] WinTracks;
        public AudioClip[] LoseTracks;

        [Range(0f, 1f)]
        public float MusicVolume = 0.22f;

        [Tooltip("Duration of crossfades between scene categories and tracks")]
        public float CrossfadeDuration = 1.6f;

        public bool DontDestroyBetweenScenes = true;

        static PurgeMusicManager s_Instance;

        AudioSource m_PrimaryAudioSource;
        AudioSource m_SecondaryAudioSource;
        AudioSource m_ActiveAudioSource;
        AudioSource m_InactiveAudioSource;
        AudioClip[] m_CurrentPlaylist;
        string m_CurrentCategory;
        int m_LastTrackIndex = -1;
        bool m_IsCrossfading;
        float m_CrossfadeStartTime;

        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
        static void Bootstrap()
        {
            if (s_Instance != null)
                return;

            GameObject prefab = Resources.Load<GameObject>("PurgeMusicManager");
            if (prefab != null)
            {
                Instantiate(prefab);
                return;
            }

            var fallback = new GameObject("PurgeMusicManager");
            fallback.AddComponent<PurgeMusicManager>();
        }

        void Awake()
        {
            if (s_Instance != null && s_Instance != this)
            {
                Destroy(gameObject);
                return;
            }

            s_Instance = this;

            if (DontDestroyBetweenScenes)
                DontDestroyOnLoad(gameObject);

            m_PrimaryAudioSource = GetComponent<AudioSource>();
            if (m_PrimaryAudioSource == null)
                m_PrimaryAudioSource = gameObject.AddComponent<AudioSource>();

            m_SecondaryAudioSource = gameObject.AddComponent<AudioSource>();
            ConfigureAudioSource(m_PrimaryAudioSource);
            ConfigureAudioSource(m_SecondaryAudioSource);

            m_ActiveAudioSource = m_PrimaryAudioSource;
            m_InactiveAudioSource = m_SecondaryAudioSource;

            EnsureDefaultResourcePlaylists();

            AudioMixerGroup group = null;
            try
            {
                group = AudioUtility.GetAudioGroup(AudioUtility.AudioGroups.HUDObjective);
            }
            catch
            {
                group = null;
            }

            if (group != null)
            {
                m_PrimaryAudioSource.outputAudioMixerGroup = group;
                m_SecondaryAudioSource.outputAudioMixerGroup = group;
            }
        }

        void EnsureDefaultResourcePlaylists()
        {
            if (!HasTracks(MenuTracks))
                MenuTracks = Resources.LoadAll<AudioClip>(MenuResourcesPath);

            if (!HasTracks(GameplayTracks))
                GameplayTracks = Resources.LoadAll<AudioClip>(GameplayResourcesPath);

            if (!HasTracks(BossTracks))
                BossTracks = Resources.LoadAll<AudioClip>(BossResourcesPath);

            if (!HasTracks(WinTracks))
                WinTracks = Resources.LoadAll<AudioClip>(WinResourcesPath);

            if (!HasTracks(LoseTracks))
                LoseTracks = Resources.LoadAll<AudioClip>(LoseResourcesPath);
        }

        void OnEnable()
        {
            SceneManager.sceneLoaded += OnSceneLoaded;
            EventManager.AddListener<GameOverEvent>(OnGameOver);
        }

        void Start()
        {
            RefreshForScene(SceneManager.GetActiveScene().name, true);
        }

        void Update()
        {
            if (m_ActiveAudioSource == null)
                return;

            UpdateCrossfade();

            if (!m_IsCrossfading && !m_ActiveAudioSource.isPlaying && m_CurrentPlaylist != null && m_CurrentPlaylist.Length > 0)
                PlayNextRandomTrack();
        }

        void OnDisable()
        {
            SceneManager.sceneLoaded -= OnSceneLoaded;
            EventManager.RemoveListener<GameOverEvent>(OnGameOver);
        }

        void OnSceneLoaded(Scene scene, LoadSceneMode mode)
        {
            RefreshForScene(scene.name, false);
        }

        void RefreshForScene(string sceneName, bool forceRestart)
        {
            AudioClip[] playlist = GetPlaylistForScene(sceneName, out string category);
            if (playlist == null || playlist.Length == 0)
                return;

            bool categoryChanged = m_CurrentCategory != category;
            m_CurrentCategory = category;
            m_CurrentPlaylist = playlist;

            if (forceRestart || categoryChanged || !m_ActiveAudioSource.isPlaying)
                PlayNextRandomTrack();
        }

        void OnGameOver(GameOverEvent evt)
        {
            string category = evt.Win ? "Win" : "Lose";
            AudioClip[] playlist = GetPlaylistForCategory(category);
            if (!HasTracks(playlist))
                return;

            m_CurrentCategory = category;
            m_CurrentPlaylist = playlist;
            PlayNextRandomTrack(true);
        }

        AudioClip[] GetPlaylistForScene(string sceneName, out string category)
        {
            sceneName = sceneName ?? string.Empty;

            if (sceneName == "IntroMenu")
            {
                category = "Menu";
                return HasTracks(MenuTracks) ? MenuTracks : GameplayTracks;
            }

            if (sceneName == "WinScene")
            {
                category = "Win";
                return HasTracks(WinTracks) ? WinTracks : MenuTracks;
            }

            if (sceneName == "LoseScene")
            {
                category = "Lose";
                return HasTracks(LoseTracks) ? LoseTracks : MenuTracks;
            }

            if (sceneName == "ReactorCavernScene")
            {
                category = "Boss";
                return HasTracks(BossTracks) ? BossTracks : GameplayTracks;
            }

            category = "Gameplay";
            return GameplayTracks;
        }

        void PlayNextRandomTrack(bool forceRestart = false)
        {
            if (!HasTracks(m_CurrentPlaylist))
                return;

            int nextIndex = 0;
            if (m_CurrentPlaylist.Length == 1)
            {
                nextIndex = 0;
            }
            else
            {
                do
                {
                    nextIndex = Random.Range(0, m_CurrentPlaylist.Length);
                }
                while (nextIndex == m_LastTrackIndex);
            }

            m_LastTrackIndex = nextIndex;
            PlayClip(m_CurrentPlaylist[nextIndex], forceRestart);
        }

        void PlayClip(AudioClip clip, bool forceRestart = false)
        {
            if (clip == null)
                return;

            if (!forceRestart && m_ActiveAudioSource != null && m_ActiveAudioSource.clip == clip && m_ActiveAudioSource.isPlaying)
                return;

            if (m_ActiveAudioSource != null && m_ActiveAudioSource.isPlaying)
            {
                m_InactiveAudioSource.clip = clip;
                m_InactiveAudioSource.volume = 0f;
                m_InactiveAudioSource.Play();
                m_IsCrossfading = true;
                m_CrossfadeStartTime = Time.unscaledTime;
            }
            else
            {
                m_ActiveAudioSource.clip = clip;
                m_ActiveAudioSource.volume = MusicVolume;
                m_ActiveAudioSource.Play();
            }
        }

        void UpdateCrossfade()
        {
            if (!m_IsCrossfading)
            {
                if (m_ActiveAudioSource != null)
                    m_ActiveAudioSource.volume = MusicVolume;
                return;
            }

            float normalized = Mathf.Clamp01((Time.unscaledTime - m_CrossfadeStartTime) / Mathf.Max(0.01f, CrossfadeDuration));
            if (m_ActiveAudioSource != null)
                m_ActiveAudioSource.volume = MusicVolume * (1f - normalized);
            if (m_InactiveAudioSource != null)
                m_InactiveAudioSource.volume = MusicVolume * normalized;

            if (normalized >= 1f)
            {
                if (m_ActiveAudioSource != null)
                {
                    m_ActiveAudioSource.Stop();
                    m_ActiveAudioSource.volume = 0f;
                }

                AudioSource previousActive = m_ActiveAudioSource;
                m_ActiveAudioSource = m_InactiveAudioSource;
                m_InactiveAudioSource = previousActive;

                if (m_ActiveAudioSource != null)
                    m_ActiveAudioSource.volume = MusicVolume;

                m_IsCrossfading = false;
            }
        }

        AudioClip[] GetPlaylistForCategory(string category)
        {
            switch (category)
            {
                case "Menu":
                    return HasTracks(MenuTracks) ? MenuTracks : GameplayTracks;
                case "Win":
                    return HasTracks(WinTracks) ? WinTracks : MenuTracks;
                case "Lose":
                    return HasTracks(LoseTracks) ? LoseTracks : MenuTracks;
                case "Boss":
                    return HasTracks(BossTracks) ? BossTracks : GameplayTracks;
                default:
                    return GameplayTracks;
            }
        }

        void ConfigureAudioSource(AudioSource source)
        {
            if (source == null)
                return;

            source.playOnAwake = false;
            source.loop = false;
            source.spatialBlend = 0f;
            source.volume = 0f;
        }

        static bool HasTracks(AudioClip[] clips)
        {
            return clips != null && clips.Length > 0;
        }
    }
}