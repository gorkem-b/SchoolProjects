using System.Collections.Generic;
using UnityEngine;

namespace Unity.FPS.Game
{
    public class ObjectiveManager : MonoBehaviour
    {
        List<Objective> m_Objectives = new List<Objective>();
        bool m_ObjectivesCompleted = false;
        bool m_HasRegisteredBlockingObjective = false;

        void Awake()
        {
            Objective.OnObjectiveCreated += RegisterObjective;
            Objective.OnObjectiveCompleted += OnObjectiveCompleted;
        }

        void RegisterObjective(Objective objective)
        {
            if (objective == null || m_Objectives.Contains(objective))
                return;

            m_Objectives.Add(objective);

            if (!objective.IsOptional)
                m_HasRegisteredBlockingObjective = true;
        }

        void OnObjectiveCompleted(Objective objective)
        {
            EvaluateObjectivesCompletion(objective);
        }

        void Update()
        {
            m_Objectives.RemoveAll(o => o == null || !o.isActiveAndEnabled);
            EvaluateObjectivesCompletion();
        }

        void EvaluateObjectivesCompletion(Objective recentlyCompleted = null)
        {
            if (m_ObjectivesCompleted || !m_HasRegisteredBlockingObjective)
                return;

            bool hasKnownBlockingObjective = recentlyCompleted != null && !recentlyCompleted.IsOptional;

            for (int i = 0; i < m_Objectives.Count; i++)
            {
                Objective objective = m_Objectives[i];
                if (objective == null || !objective.isActiveAndEnabled)
                    continue;

                if (!objective.IsOptional)
                    hasKnownBlockingObjective = true;

                if (objective.IsBlocking())
                    return;
            }

            if (!hasKnownBlockingObjective)
                return;

            m_ObjectivesCompleted = true;
            EventManager.Broadcast(Events.AllObjectivesCompletedEvent);
        }

        void OnDestroy()
        {
            Objective.OnObjectiveCreated -= RegisterObjective;
            Objective.OnObjectiveCompleted -= OnObjectiveCompleted;
        }
    }
}