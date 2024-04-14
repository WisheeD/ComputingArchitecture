import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv(
    '../Task/cmake-build-debug/values.csv',
    header=None,
    names=['N', 'Time']
)

plt.figure(figsize=(10, 6))

plt.plot(data['Time'], data['N'], marker='o')

plt.title('Comparison of Time and Iterations')
plt.xlabel('Time')
plt.ylabel('Iterations')
plt.grid(True)
plt.show()
