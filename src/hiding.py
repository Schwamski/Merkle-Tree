import pandas as pd
import matplotlib.pyplot as plt
import statsmodels.api as sm
from scipy.stats import linregress

# Create binary features
def int_to_padded_binary(x, bits=16):
    return format(x, '0{}b'.format(bits))

# load df and expand for each bit in the message
data = pd.read_csv('merkle_data.csv')
binary_features = data['BinaryMessage'].apply(lambda x: pd.Series(list(int_to_padded_binary(x)))).astype(int)
binary_features.columns = [f'bit_{i}' for i in range(16)]
data = pd.concat([data, binary_features], axis=1)

# multivariate regression for each bit vs commitment
Y = data['RootHash']
X = data[[f'bit_{i}' for i in range(16)]]
X = sm.add_constant(X)
model = sm.OLS(Y, X).fit()
print(model.summary())

# regression for the input message vs commitment hash
slope, intercept, r_value, p_value, std_err = linregress(data['BinaryMessage'], data['RootHash'])

# figure
fig, axs = plt.subplots(1, 2, figsize=(16, 6))

axs[0].scatter(data['BinaryMessage'], data['RootHash'])
axs[0].plot(data['BinaryMessage'], intercept + slope * data['BinaryMessage'], 'r')
axs[0].set_xlabel('Binary Message (as integer)')
axs[0].set_ylabel('Root Hash')
axs[0].set_title('Root Hash vs. Binary Message')

axs[1].hist(data['RootHash'], bins=40, edgecolor='black')
axs[1].set_xlabel('Root Hash')
axs[1].set_ylabel('Frequency')
axs[1].set_title('Root Hash Distribution')

# display plots
plt.tight_layout()
plt.show()