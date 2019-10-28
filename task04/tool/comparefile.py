import sys

def comparefile(filename1, filename2):
	print("Compare", filename1, filename2, ":\t", end = "")
	with open(filename1, "r") as f1:
		with open(filename2, "r") as f2:
			line1 = f1.readline()
			line2 = f2.readline()
			while line1 != '' and line2 != '':
				if line1 != line2:
					return 0
				line1 = f1.readline()
				line2 = f2.readline()
			if line1 != '' or line2 != '':
				return 0
	return 1

def main():
	if comparefile(sys.argv[1], sys.argv[2]) == 1:
		print("Success!")
	else:
		print("Failed!")

if __name__ == '__main__':
	main()


