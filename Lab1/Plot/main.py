import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv(
    '../Task/cmake-build-debug/values.csv',
    header=None,
    names=['Time', 'N']
)

plt.figure(figsize=(10, 6))

plt.plot(data['Time'], data['N'], label='function', marker='o', linestyle='-')

plt.title('Comparison of Time and N')
plt.xlabel('Time')
plt.ylabel('N')
plt.legend()
plt.grid(True)
plt.show()
