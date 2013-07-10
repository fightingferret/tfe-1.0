CC      = gcc
PROF    = 
DEBUG   = -ggdb
WARN    = -Wall
CFLAGS  = -ansi $(DEBUG) -O $(WARN) $(PROF) 
C_FLAGS = $(CFLAGS) 
VPATH   = ./OBJ

DIR = ./OBJ

O_FILES = $(C_FILES:.cc=.o)

C_FILES = abil.cc account.cc acid.cc act.cc action.cc affect.cc alias.cc \
          align.cc appear.cc area.cc armor.cc auction.cc  \
          ban.cc bank.cc bash.cc bow.cc bug.cc cards.cc channel.cc \
          char.cc char_name.cc clan.cc  \
          cleric.cc code.cc code2.cc cold.cc color.cc const.cc corpse.cc  \
          custom.cc damage.cc death.cc descr.cc display.cc   \
          drink.cc \
          edit.cc enemy.cc exit.cc event.cc event2.cc  \
          exp.cc familiar.cc fight.cc fight2.cc  \
          fight3.cc file.cc find.cc fire.cc flag.cc flee.cc  \
          food.cc get.cc give.cc group.cc \
          handler.cc help.cc hide.cc host.cc  \
          imm.cc interp.cc kick.cc leech.cc level.cc library.cc \
          lib_exit.cc \
          lib_fight.cc lib_logic.cc lib_object.cc lib_quest.cc \
          light.cc list.cc log.cc login.cc look.cc \
          macros.cc mage.cc magic.cc magic2.cc \
          memory.cc misc.cc mob_imm.cc  \
          mobile.cc money.cc  \
          monk.cc mount.cc move.cc move2.cc \
          mprog.cc network.cc newbie.cc \
          note.cc number.cc obj_imm.cc obj_name.cc obj_prop.cc \
          obj_sort.cc object.cc object2.cc object3.cc \
          oprog.cc option.cc output.cc paladin.cc path.cc player.cc \
          pet.cc pos.cc  \
          program.cc prompt.cc psi.cc put.cc quest.cc  \
          ranger.cc recipe.cc recog.cc regen.cc relig.cc \
          reput.cc rescue.cc  \
	  reset.cc resist.cc room.cc rtable.cc save.cc score.cc \
          search.cc set.cc shock.cc \
          shop.cc skill.cc social.cc  \
          spell.cc startup.cc string.cc switch.cc table.cc teleport.cc \
          tfe.cc thief.cc thing.cc \
          throw.cc time.cc track.cc trainer.cc undead.cc \
          update.cc variable.cc verb.cc vote.cc wand.cc water.cc wear.cc \
          weather.cc weight.cc where.cc who.cc

all: $(O_FILES)
	rm -f tfe
	$(CC) $(C_FLAGS) -o tfe $(DIR)/*.o $(L_FLAGS)
	rm -f daemon         
	$(CC) $(C_FLAGS) -o daemon daemon.cc $(L_FLAGS)

.cc.o: 
	$(CC) -c $(C_FLAGS) -I. $< -o $(DIR)/$@











