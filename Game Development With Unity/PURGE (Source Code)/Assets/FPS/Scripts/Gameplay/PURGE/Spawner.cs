using System.Collections;
using Unity.FPS.Game;
using UnityEngine;
using UnityEngine.AI;

namespace PURGE.Gameplay
{
    public class Spawner : MonoBehaviour
    {
        [Tooltip("Enemy prefab to spawn")]
        public GameObject EnemyPrefab;

        [Tooltip("Spawn points used by this spawner. If empty, this object's transform is used")]
        public Transform[] SpawnPoints;

        [Tooltip("Seconds between each wave")]
        public float SpawnInterval = 20f;

        [Tooltip("How many enemies are spawned per wave")]
        public int EnemiesPerWave = 1;

        [Tooltip("Boss object reference. Spawning stops when this boss dies")]
        public GameObject BossReference;

        [Tooltip("Start spawning automatically on Start")]
        public bool SpawnOnStart = true;

        [Header("Warning")]
        [Tooltip("How long before the spawn a warning marker should be shown")]
        public float WarningLeadTime = 1.6f;

        [Tooltip("World radius of the spawn warning marker")]
        public float WarningRadius = 4f;

        [Tooltip("Whether this spawner should broadcast a HUD message before spawning")]
        public bool DisplayWarningMessage = true;

        [Tooltip("HUD message shown when the warning starts")]
        public string WarningMessage = "Incoming reinforcements";

        [Tooltip("Minimum delay between global warning HUD messages from spawners")]
        public float WarningMessageCooldown = 1.5f;

        [ColorUsage(false, true)]
        [Tooltip("Visual color of the spawn warning marker")]
        public Color WarningColor = new Color(1f, 0.42f, 0.12f, 0.9f);

        [Tooltip("Point light intensity of the spawn warning marker")]
        public float WarningLightIntensity = 5.5f;

        [Tooltip("Point light range of the spawn warning marker")]
        public float WarningLightRange = 8f;

        Coroutine m_SpawnRoutine;
        Health m_BossHealth;
        bool m_ObservedBossAlive;
        static float s_LastGlobalWarningMessageTime = Mathf.NegativeInfinity;

        void Start()
        {
            CacheBossHealth();
            m_ObservedBossAlive = IsBossAlive();

            if (SpawnOnStart)
            {
                BeginSpawning();
            }
        }

        public void BeginSpawning()
        {
            if (m_SpawnRoutine != null)
                return;

            m_SpawnRoutine = StartCoroutine(SpawnLoop());
        }

        public void StopSpawning()
        {
            if (m_SpawnRoutine == null)
                return;

            StopCoroutine(m_SpawnRoutine);
            m_SpawnRoutine = null;
        }

        IEnumerator SpawnLoop()
        {
            float interval = Mathf.Max(0.1f, SpawnInterval);
            float warningLead = Mathf.Clamp(WarningLeadTime, 0f, interval);
            WaitForSeconds preWarningWait = warningLead < interval
                ? new WaitForSeconds(interval - warningLead)
                : null;
            WaitForSeconds warningWait = warningLead > 0f
                ? new WaitForSeconds(warningLead)
                : null;

            while (IsBossAlive())
            {
                m_ObservedBossAlive = true;

                if (preWarningWait != null)
                    yield return preWarningWait;

                if (!IsBossAlive())
                    break;

                Transform[] waveSpawnPoints = SelectSpawnPointsForWave();
                TriggerSpawnWarning(waveSpawnPoints, warningLead);

                if (warningWait != null)
                    yield return warningWait;

                if (!IsBossAlive())
                    break;

                SpawnWave(waveSpawnPoints);
            }

            m_SpawnRoutine = null;
        }

        Transform[] SelectSpawnPointsForWave()
        {
            if (SpawnPoints == null || SpawnPoints.Length == 0)
                return null;

            int spawnCount = Mathf.Clamp(EnemiesPerWave, 1, SpawnPoints.Length);
            int startIndex = Random.Range(0, SpawnPoints.Length);
            Transform[] selectedPoints = new Transform[spawnCount];
            for (int i = 0; i < spawnCount; i++)
                selectedPoints[i] = SpawnPoints[(startIndex + i) % SpawnPoints.Length];

            return selectedPoints;
        }

        void SpawnWave(Transform[] selectedSpawnPoints)
        {
            if (EnemyPrefab == null)
                return;

            if (selectedSpawnPoints != null && selectedSpawnPoints.Length > 0)
            {
                for (int i = 0; i < selectedSpawnPoints.Length; i++)
                {
                    Transform point = selectedSpawnPoints[i];
                    if (point != null)
                        SpawnEnemyAt(point.position, point.rotation);
                }

                return;
            }

            SpawnEnemyAt(transform.position, transform.rotation);
        }

        void SpawnEnemyAt(Vector3 desiredPosition, Quaternion desiredRotation)
        {
            bool requiresNavMesh = PrefabRequiresNavMesh();
            Vector3 spawnPosition = desiredPosition;
            float hoverOffset = 0f;

            if (requiresNavMesh)
            {
                if (!TryFindSpawnPositionOnNavMesh(desiredPosition, out spawnPosition, out hoverOffset))
                    return;
            }

            GameObject spawnedEnemy = Instantiate(EnemyPrefab, spawnPosition, desiredRotation);
            if (spawnedEnemy == null)
                return;

            if (!spawnedEnemy.activeSelf)
                spawnedEnemy.SetActive(true);

            if (requiresNavMesh)
                ConfigureSpawnedNavMeshAgent(spawnedEnemy, spawnPosition, hoverOffset);
        }

        bool PrefabRequiresNavMesh()
        {
            return EnemyPrefab != null && EnemyPrefab.GetComponentInChildren<NavMeshAgent>(true) != null;
        }

        bool TryFindSpawnPositionOnNavMesh(Vector3 desiredPosition, out Vector3 navMeshPosition, out float hoverOffset)
        {
            NavMeshHit navHit;
            bool foundNavMesh =
                NavMesh.SamplePosition(desiredPosition, out navHit, 36f, NavMesh.AllAreas)
                || NavMesh.SamplePosition(desiredPosition + Vector3.down * 4f, out navHit, 36f, NavMesh.AllAreas)
                || NavMesh.SamplePosition(desiredPosition + Vector3.up * 2f, out navHit, 36f, NavMesh.AllAreas);

            if (!foundNavMesh)
            {
                navMeshPosition = desiredPosition;
                hoverOffset = 0f;
                return false;
            }

            navMeshPosition = navHit.position;
            hoverOffset = Mathf.Max(0f, desiredPosition.y - navHit.position.y);
            return true;
        }

        void ConfigureSpawnedNavMeshAgent(GameObject spawnedEnemy, Vector3 navMeshPosition, float hoverOffset)
        {
            if (spawnedEnemy == null)
                return;

            NavMeshAgent agent = spawnedEnemy.GetComponent<NavMeshAgent>();
            if (agent == null)
                return;

            if (!agent.enabled)
                agent.enabled = true;

            if (!agent.isOnNavMesh)
            {
                if (!agent.Warp(navMeshPosition) && !agent.isOnNavMesh)
                {
                    agent.enabled = false;
                    return;
                }
            }

            agent.baseOffset = Mathf.Max(agent.baseOffset, hoverOffset);
            agent.nextPosition = navMeshPosition;
            if (agent.isOnNavMesh)
                agent.isStopped = false;
        }

        void TriggerSpawnWarning(Transform[] selectedSpawnPoints, float warningLead)
        {
            if (warningLead > 0f)
            {
                if (selectedSpawnPoints != null && selectedSpawnPoints.Length > 0)
                {
                    for (int i = 0; i < selectedSpawnPoints.Length; i++)
                    {
                        if (selectedSpawnPoints[i] != null)
                            StartCoroutine(PlaySpawnWarning(selectedSpawnPoints[i].position, warningLead));
                    }
                }
                else
                {
                    StartCoroutine(PlaySpawnWarning(transform.position, warningLead));
                }
            }

            if (DisplayWarningMessage
                && !string.IsNullOrEmpty(WarningMessage)
                && Time.time >= s_LastGlobalWarningMessageTime + Mathf.Max(0.1f, WarningMessageCooldown))
            {
                s_LastGlobalWarningMessageTime = Time.time;
                DisplayMessageEvent displayMessage = Events.DisplayMessageEvent;
                displayMessage.Message = WarningMessage;
                displayMessage.DelayBeforeDisplay = 0f;
                EventManager.Broadcast(displayMessage);
            }
        }

        IEnumerator PlaySpawnWarning(Vector3 worldPosition, float duration)
        {
            GameObject marker = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            marker.name = "SpawnWarningMarker";
            marker.transform.position = worldPosition + Vector3.up * 0.05f;
            marker.transform.rotation = Quaternion.identity;

            Collider markerCollider = marker.GetComponent<Collider>();
            if (markerCollider != null)
                Destroy(markerCollider);

            Renderer markerRenderer = marker.GetComponent<Renderer>();
            if (markerRenderer != null)
            {
                markerRenderer.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
                markerRenderer.receiveShadows = false;
                Shader shader = Shader.Find("Sprites/Default");
                if (shader == null)
                    shader = Shader.Find("Universal Render Pipeline/Unlit");
                if (shader != null)
                {
                    Material markerMaterial = new Material(shader);
                    markerMaterial.color = WarningColor;
                    markerRenderer.material = markerMaterial;
                }
            }

            GameObject lightObject = new GameObject("SpawnWarningLight");
            lightObject.transform.position = worldPosition + Vector3.up * 1.5f;
            Light warningLight = lightObject.AddComponent<Light>();
            warningLight.type = LightType.Point;
            warningLight.color = WarningColor;
            warningLight.range = WarningLightRange;
            warningLight.intensity = 0f;
            warningLight.shadows = LightShadows.None;

            Vector3 startScale = new Vector3(Mathf.Max(0.5f, WarningRadius * 0.35f), 0.04f, Mathf.Max(0.5f, WarningRadius * 0.35f));
            Vector3 endScale = new Vector3(WarningRadius, 0.04f, WarningRadius);
            float elapsed = 0f;
            float safeDuration = Mathf.Max(0.05f, duration);

            while (elapsed < safeDuration)
            {
                elapsed += Time.deltaTime;
                float normalized = Mathf.Clamp01(elapsed / safeDuration);
                float pulse = 0.5f + 0.5f * Mathf.Sin(normalized * Mathf.PI * 4f);

                marker.transform.localScale = Vector3.Lerp(startScale, endScale, normalized);

                if (markerRenderer != null && markerRenderer.material != null)
                {
                    Color color = WarningColor;
                    color.a *= Mathf.Lerp(0.95f, 0.2f, normalized) * Mathf.Lerp(0.8f, 1f, pulse);
                    markerRenderer.material.color = color;
                }

                warningLight.intensity = WarningLightIntensity * Mathf.Lerp(0.8f, 1f, pulse) * (1f - normalized * 0.45f);
                yield return null;
            }

            if (marker != null)
                Destroy(marker);
            if (lightObject != null)
                Destroy(lightObject);
        }

        bool IsBossAlive()
        {
            if (BossReference == null)
                return false;

            if (m_BossHealth == null)
            {
                CacheBossHealth();
            }

            if (m_BossHealth != null)
            {
                return m_BossHealth.CurrentHealth > 0f;
            }

            return BossReference.activeInHierarchy;
        }

        void CacheBossHealth()
        {
            if (BossReference != null)
            {
                m_BossHealth = BossReference.GetComponent<Health>();
            }
        }
    }
}
