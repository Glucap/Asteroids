function AsteroidNamer()

	NameLenght = RandomRange(4, 8)

	name = MakeName("FemaleNames.txt", 1) .. "-" .. RandomRange(0, 9999999)
	
	return name

end