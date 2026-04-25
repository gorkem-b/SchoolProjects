export const taskStatusOptions = [
  { value: 'ToRead', label: 'To Read' },
  { value: 'Completed', label: 'Completed' },
]

export const taskStatusLabels = taskStatusOptions.reduce((labels, option) => {
  labels[option.value] = option.label
  return labels
}, {})