cpp      = gcc
PROF    = 
DEBUG   = -ggdb
WARN    = -Wall
CFLAGS  = -ansi $(DEBUG) -O $(WARN) $(PROF) 
C_FLAGS = $(CFLAGS) 
VPATH   = ./OBJ

DIR = ./OBJ

O_FILES = $(C_FILES:.cpp=.o)

C_FILES = abil.cpp account.cpp acid.cpp act.cpp action.cpp affect.cpp alias.cpp \
          align.cpp appear.cpp area.cpp armor.cpp auction.cpp  \
          ban.cpp bank.cpp bash.cpp bow.cpp bug.cpp cards.cpp channel.cpp \
          char.cpp char_name.cpp clan.cpp  \
          cleric.cpp code.cpp code2.cpp cold.cpp color.cpp const.cpp corpse.cpp  \
          custom.cpp damage.cpp death.cpp descr.cpp display.cpp   \
          drink.cpp \
          edit.cpp enemy.cpp exit.cpp event.cpp event2.cpp  \
          exp.cpp familiar.cpp fight.cpp fight2.cpp  \
          fight3.cpp file.cpp find.cpp fire.cpp flag.cpp flee.cpp  \
          food.cpp get.cpp give.cpp group.cpp \
          handler.cpp help.cpp hide.cpp host.cpp  \
          imm.cpp interp.cpp kick.cpp leech.cpp level.cpp library.cpp \
          lib_exit.cpp \
          lib_fight.cpp lib_logic.cpp lib_object.cpp lib_quest.cpp \
          light.cpp list.cpp log.cpp login.cpp look.cpp \
          macros.cpp mage.cpp magic.cpp magic2.cpp \
          memory.cpp misc.cpp mob_imm.cpp  \
          mobile.cpp money.cpp  \
          monk.cpp mount.cpp move.cpp move2.cpp \
          mprog.cpp network.cpp newbie.cpp \
          note.cpp number.cpp obj_imm.cpp obj_name.cpp obj_prop.cpp \
          obj_sort.cpp object.cpp object2.cpp object3.cpp \
          oprog.cpp option.cpp output.cpp paladin.cpp path.cpp player.cpp \
          pet.cpp pos.cpp  \
          program.cpp prompt.cpp psi.cpp put.cpp quest.cpp  \
          ranger.cpp recipe.cpp recog.cpp regen.cpp relig.cpp \
          reput.cpp rescue.cpp  \
	  reset.cpp resist.cpp room.cpp rtable.cpp save.cpp score.cpp \
          search.cpp set.cpp shock.cpp \
          shop.cpp skill.cpp social.cpp  \
          spell.cpp startup.cpp string.cpp switch.cpp table.cpp teleport.cpp \
          tfe.cpp thief.cpp thing.cpp \
          throw.cpp time.cpp track.cpp trainer.cpp undead.cpp \
          update.cpp variable.cpp verb.cpp vote.cpp wand.cpp water.cpp wear.cpp \
          weather.cpp weight.cpp where.cpp who.cpp

all: $(O_FILES)
	$(cpp) $(C_FLAGS) -o ../bin/tfe $(DIR)/*.o $(L_FLAGS)
	$(cpp) $(C_FLAGS) -o ../bin/daemon daemon.cpp $(L_FLAGS)

abil.o: abil.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
account.o: account.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
acid.o: acid.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
act.o: act.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
action.o: action.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
affect.o: affect.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
alias.o: alias.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
align.o: align.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
appear.o: appear.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
area.o: area.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
armor.o: armor.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
auction.o: auction.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
ban.o: ban.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
bank.o: bank.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
bash.o: bash.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
bow.o: bow.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
bug.o: bug.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
cards.o: cards.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
channel.o: channel.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
char.o: char.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
char_name.o: char_name.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
clan.o: clan.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
cleric.o: cleric.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
code.o: code.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
code2.o: code2.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
cold.o: cold.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
color.o: color.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
const.o: const.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
corpse.o: corpse.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
custom.o: custom.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
damage.o: damage.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
death.o: death.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
descr.o: descr.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
display.o: display.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
drink.o: drink.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
edit.o: edit.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
enemy.o: enemy.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
exit.o: exit.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
event.o: event.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
event2.o: event2.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
exp.o: exp.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
familiar.o: familiar.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
fight.o: fight.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
fight2.o: fight2.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
fight3.o: fight3.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
file.o: file.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
find.o: find.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
fire.o: fire.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
flag.o: flag.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
flee.o: flee.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
food.o: food.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
get.o: get.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
give.o: give.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
group.o: group.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
handler.o: handler.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
help.o: help.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
hide.o: hide.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
host.o: host.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
imm.o: imm.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
interp.o: interp.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
kick.o: kick.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
leech.o: leech.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
level.o: level.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
library.o: library.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
lib_exit.o: lib_exit.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
lib_fight.o: lib_fight.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
lib_logic.o: lib_logic.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
lib_object.o: lib_object.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
lib_quest.o: lib_quest.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
light.o: light.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
list.o: list.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
log.o: log.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
login.o: login.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
look.o: look.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
macros.o: macros.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
mage.o: mage.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
magic.o: magic.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
magic2.o: magic2.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
memory.o: memory.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
misc.o: misc.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
mob_imm.o: mob_imm.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
mobile.o: mobile.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
money.o: money.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
monk.o: monk.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
mount.o: mount.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
move.o: move.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
move2.o: move2.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
mprog.o: mprog.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
network.o: network.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
newbie.o: newbie.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
note.o: note.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
number.o: number.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
obj_imm.o: obj_imm.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
obj_name.o: obj_name.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
obj_prop.o: obj_prop.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
obj_sort.o: obj_sort.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
object.o: object.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
object2.o: object2.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
object3.o: object3.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
oprog.o: oprog.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
option.o: option.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
output.o: output.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
paladin.o: paladin.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
path.o: path.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
player.o: player.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
pet.o: pet.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
pos.o: pos.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
program.o: program.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
prompt.o: prompt.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
psi.o: psi.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
put.o: put.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
quest.o: quest.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
ranger.o: ranger.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
recipe.o: recipe.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
recog.o: recog.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
regen.o: regen.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
relig.o: relig.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
reput.o: reput.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
rescue.o: rescue.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
reset.o: reset.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
resist.o: resist.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
room.o: room.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
rtable.o: rtable.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
save.o: save.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
score.o: score.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
search.o: search.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
set.o: set.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
shock.o: shock.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
shop.o: shop.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
skill.o: skill.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
social.o: social.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
spell.o: spell.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
startup.o: startup.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
string.o: string.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
switch.o: switch.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
table.o: table.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
teleport.o: teleport.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
tfe.o: tfe.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
thief.o: thief.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
thing.o: thing.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
throw.o: throw.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
time.o: time.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
track.o: track.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
trainer.o: trainer.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
undead.o: undead.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
update.o: update.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
variable.o: variable.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
verb.o: verb.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
vote.o: vote.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
wand.o: wand.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
water.o: water.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
wear.o: wear.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
weather.o: weather.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
weight.o: weight.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
where.o: where.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
who.o: who.cpp
	$(cpp) -c $(C_FLAGS) -I. $< -o $(DIR)/$@
