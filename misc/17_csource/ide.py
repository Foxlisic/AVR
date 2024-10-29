# -*- coding: utf-8 -*-

import curses
import locale

from curses import wrapper

# Коды клавиш
# $104 left
# $105 right
# $103 top
# $102 down
# $109-$114 f1-f12
# $09  tab
# $107 <- back

# ПАМЯТКИ
# curses.color_pair(i) + curses.A_BOLD + curses.A_REVERSE  (как проставить цвета)

locale.setlocale(locale.LC_ALL, 'C.UTF-8')

text_skip 	= 0 # Максимальное количество линии
text_rows 	= 1 # Отступ сверху
cursor_x    = 0 # Где находится курсор
cursor_y    = 0

# Нарисовать типовую рамку
def cborder(o,x1,y1,x2,y2):

	for i in range(x1,x2): 

		o.addstr(y1,   i, '=')
		o.addstr(y2-1, i, '=')

	for i in range(y1+1, y2-1): 

		o.addstr(i, x1,   '|')
		o.addstr(i, x2-1, '|')

# Конвертировать в hex 8 bit
def tohex(i):

	h = "0123456789ABCDEF"
	return h[(i / 16) % 16] + h[i % 16]

# Перерисовка линии
def repaint_ln(scr, i):

	global dx, code

	t = code[i + text_skip]

	# Вывод строки
	scr.addstr(i + 2, 2, t + " " * (dx - len(code[i + text_skip]) - 43))

	# Рисование строки в цветах
	# --- разбор кодовых слов

	if i == cursor_y: # Подсветка текущего положения курсора

		if cursor_x < len(t): 
			s = t[cursor_x]
		else: 
			s = " "

		scr.addstr(2 + i, 2 + cursor_x, s, curses.A_REVERSE)


# Перерисовка окна текста
def repaint_text(scr):	

	global code, dx, dy, cursor_x, cursor_y

	for i in range(2,dy-3):

		y = i - 2 # Коррекция

		# Показывать "~" на незаполненных участках, как в vim
		if (text_skip + y >= text_rows):
			scr.addstr(i,1,"~ " + " " * (dx - 44), curses.color_pair(4))

		else:
			scr.addstr(i,1,"  ")
			repaint_ln(scr, y)

	# Строка статуса
	scr.addstr(dy-2, 2, " " + str(cursor_y + text_skip) + ":" + str(cursor_x) + "/" + str(text_rows) + " ")

# Перерисовка всего окна
def repaint(scr):

	wrx = dx - 40

	cborder(scr, 0,   0, dx-1, dy-1)    # Общее окно
	cborder(scr, wrx, 0, dx-1, dy-1)    # Окно регистров, стека и так далее
	
	# Нарисовать регистры
	scr.addstr(0, 1, " RCISC IDE ", curses.A_REVERSE)
	scr.addstr(2, wrx + 2, " Регистры ")
	scr.addstr(3, wrx + 2, "==========")
	scr.addstr(5, wrx + 2, "r0 00   r4 00   r8 00  yl  00")
	scr.addstr(6, wrx + 2, "r1 00   r5 00   r9 00  yh  00")
	scr.addstr(7, wrx + 2, "r2 00   r6 00   xl 00  spl 00")
	scr.addstr(8, wrx + 2, "r3 00   r7 00   xh 00  sph 00")

 	for i in range(0, 4):

		scr.addstr(5, wrx + 5 + 8*i, tohex(r[i*4  ]), curses.A_BOLD)
		scr.addstr(6, wrx + 5 + 8*i, tohex(r[i*4+1]), curses.A_BOLD)
		scr.addstr(7, wrx + 5 + 8*i, tohex(r[i*4+2]), curses.A_BOLD)
		scr.addstr(8, wrx + 5 + 8*i, tohex(r[i*4+3]), curses.A_BOLD)

	repaint_text(scr)
	scr.refresh()

# Главный скрипт
def main(scr):

	global dx, dy, cursor_x, cursor_y, text_skip, code, text_rows

	scr.clear()

	curses.noecho()
	curses.cbreak()
	curses.curs_set(0)
	scr.keypad(1)

	dy,dx = scr.getmaxyx()

	# Цвета должны поддерживаться в консоли	
	if curses.has_colors():

		curses.use_default_colors() 
		curses.init_pair(1, 1, 0) # Красный
		curses.init_pair(2, 2, 0) # Зеленый
		curses.init_pair(3, 3, 0) # Коричневый
		curses.init_pair(4, 4, 0) # Синий
		curses.init_pair(5, 5, 0) # Сиреневый
		curses.init_pair(6, 6, 0) # Бирюза
		curses.init_pair(7, 7, 0) # Серый

	else: return   

	repaint(scr)

	x, y = 0, 0

	while (True):

		k = scr.getch()
		m = text_skip + cursor_y

		# Влево
		# ------------------------------------
		if k == 0x104:   
			
			if cursor_x > 0:
				cursor_x -= 1

			repaint_ln(scr, cursor_y)

		# Вправо
		# ------------------------------------
		elif k == 0x105: 

			if cursor_x < dx - 44:
				cursor_x += 1

			repaint_ln(scr, cursor_y)

		# Вверх
		# ------------------------------------
		elif k == 0x103: 

			if cursor_y > 0:
				cursor_y -= 1
			else:
				if text_skip > 0: 
					text_skip -= 1
			
			repaint_text(scr) # Перерисовка всего экрана

		# Вниз
		# ------------------------------------
		elif k == 0x102: 

			cursor_y += 1 
			m = m + 1

			# Добавить новую строку, если ее еще нет
			if (m >= text_rows):

				code.append("")
				text_rows += 1

			repaint_text(scr) # Перерисовка всего экрана

		# F10
		# ------------------------------------
		elif k == 0x112: 
			break

		# Backspace
		# ------------------------------------
		elif k == 0x107:

			if cursor_x > 0:

				code[m] = code[m][0:cursor_x-1] + code[m][cursor_x:]
				cursor_x -= 1

			#else:

				# перенос строки выше

			repaint_ln(scr, cursor_y)

		# ENTER
		# ------------------------------------
		elif k == 10:

			code.append("")

			# Перенос строк
			for i in range(text_rows - 1, m, -1):				
				code[i+1] = code[i]

			# Разделение фразы посередине для переноса
			code[m+1] = code[m][cursor_x:]
			code[m]   = code[m][0:cursor_x]

			cursor_x  = 0
			cursor_y  = cursor_y + 1
			text_rows = text_rows + 1

			repaint_text(scr) # Перерисовка всего экрана

		# PAGEUP
		# PAGEDOWN
		
		# Клавиша HOME
		# ------------------------------------
		elif k == 262:

			cursor_x = 0
			repaint_ln(scr, cursor_y)

		# Клавиша END
		# ------------------------------------
		elif k == 360:

			cursor_x = len(code[m])
			repaint_ln(scr, cursor_y)

		# Это буква + распознать русскую букву
		# ------------------------------------
		elif k < 256:

			char = chr(k)

			# Доработать получение UTF-8 символов
			#if k >= 128:
			#	a = scr.getch()
			#	char = char + chr(a)
		
			# Вставка символа
			code[m] = code[m][0:cursor_x] + char + code[m][cursor_x:]
			cursor_x += 1

			repaint_ln(scr, cursor_y)

		#scr.addstr(l,0,str(k))

# -----------------------------------------------------------------------

dx, dy = 0, 0     # Размеры экрана

r = [] # Объявление глобальной переменной регистры
prg = [] # Программный код
vid = [] # Видеопамять
code = [] # Строки программного кода перед компиляцией
code.append("BEGIN")

for i in range(0,16): r.append(0)
for i in range(0,32768): prg.append(0)
for i in range(0,10240): vid.append(0)

# Использовать curses wrapper для отладки
wrapper(main)