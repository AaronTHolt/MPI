from csv import DictReader, DictWriter
from sets import Set
import re, string
import ast


replace_punctuation = string.maketrans(string.punctuation, ' '*len(string.punctuation))
# text = text.translate(replace_punctuation)

exclude = set(string.punctuation)

data = {} 
old = DictReader(open("train.csv", 'r'))
for ii in old:
	# answers=ast.literal_eval(ii['Answer'])
	qid = ast.literal_eval(ii['id'])
	pruned_text = ii['text'].lower().translate(replace_punctuation)
	pruned_text = re.sub('\t', ' ', pruned_text)
	data[qid] = [pruned_text, ii['cat']]

	# print qid, data[qid]


# Write predictions
o = DictWriter(open('train_pruned.csv', 'wb'), ['id', 'text', 'cat'])
o.writeheader()
for ii in data:
    o.writerow({'id': ii, 'text': data[ii][0], 'cat': data[ii][1]})
