import random

random_words = [ "scale","dressing","despair""agony","creed","realize","district","east","shrink","justify","charm","suntan","harmony","sodium","strong","tail","soldier","declaration","site","embarrassment","hay","professional","time",
    "sunrise","fraud","contradiction","innocent","tense","fuss","lily","fear","person","railcar","exact","confession","biography","resign","deny","tradition","freckle","journal","shame","domination","index","bedroom","range",
    "perfume","whisper","social","chin","advice","trial","understand","tough","appearance","function","private","economics","overeat","brain","average","stable","launch",
    "notebook","dough","asylum","sleep","bomb","dedicate","distort","brilliance","shorts","string","net","layer","jaw","install","long","text","jury","buffet","crowd","wind","care","feast","center","damage","penalty","tension","architect",
    "sofa","poison","distribute","broken","cattle","constellation","follow","deer","irony","suite"]

def insert_word():
    words = 0
    limit = 2999
    add_enter_key = 30
    num_words = []
    while words <= limit:
        words+= 1
        word = random.choice(random_words)
        if(words == add_enter_key):
            add_enter_key += 30
            num_words.append("\n")
        else:
            num_words.append(word + " ")

    return num_words

if __name__ == '__main__':
    counter = 0

    while counter <= 9:
        counter+= 1
        name_file = "file" + str(counter) + ".txt"
        path = "/home/antoniofasulo/Scrivania/Random_File/files/"
        absolute_path = path + name_file
        file = open(absolute_path, 'w')
        words = insert_word()
        file.writelines(words)
        file.close()





