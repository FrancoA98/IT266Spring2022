#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

const int SHOTGUN_MOD_AMMO = BIT(0);

class rvWeaponShotgun : public rvWeapon {
public:

	CLASS_PROTOTYPE(rvWeaponShotgun);

	rvWeaponShotgun(void);

	virtual void			Spawn(void);
	void					Save(idSaveGame* savefile) const;
	void					Restore(idRestoreGame* savefile);
	void					PreSave(void);
	void					PostSave(void);

protected:
	//int						hitscans; //MOD1 TAKEN: No hitscans
	float					range;
	void					Attack		( int );

private:

	stateResult_t		State_Idle(const stateParms_t& parms);
	stateResult_t		State_Fire(const stateParms_t& parms);
	//stateResult_t		State_Reload(const stateParms_t& parms);//MOD1 TAKEN: Not requiring reload

	CLASS_STATES_PROTOTYPE(rvWeaponShotgun);
};

CLASS_DECLARATION(rvWeapon, rvWeaponShotgun)
END_CLASS

/*
================
rvWeaponShotgun::rvWeaponShotgun
================
*/
rvWeaponShotgun::rvWeaponShotgun(void) {
}

/*
================
rvWeaponShotgun::Spawn
================
*/
void rvWeaponShotgun::Spawn(void) {
	//hitscans = spawnArgs.GetFloat("hitscans");//MOD1 TAKEN: No hitscan usage
	SetState("Raise", 0);
	range = spawnArgs.GetFloat("range", "32");//MOD1 ADDED: Defined variable for range calculation
}

/*
================
rvWeaponShotgun::Save
================
*/
void rvWeaponShotgun::Save(idSaveGame* savefile) const {
	savefile->WriteFloat(range);//MOD1 ADDED: Added range value to the save file
}

/*
================
rvWeaponShotgun::Restore
================
*/
void rvWeaponShotgun::Restore(idRestoreGame* savefile) {
	//hitscans = spawnArgs.GetFloat("hitscans"); //MOD1 TAKEN: Not using bullets
	savefile->ReadFloat(range);
}

/*
================
rvWeaponShotgun::PreSave
================
*/
void rvWeaponShotgun::PreSave(void) {
}

/*
================
rvWeaponShotgun::PostSave
================
*/
void rvWeaponShotgun::PostSave(void) {
}

void rvWeaponShotgun::Attack ( int nAT ) {
	gameLocal.Printf("Inside Attack function\n");
	//MOD1 ADDED START
	trace_t	tr; //Trace involved in damage application
	idEntity* ent; //Involved in damage application
	gameLocal.TracePoint(owner, tr,
		playerViewOrigin,
		playerViewOrigin + playerViewAxis[0] * range,
		MASK_SHOT_RENDERMODEL, owner);

	owner->WeaponFireFeedback(&weaponDef->dict);//I know this looks for something in the def file
	ent = gameLocal.entities[tr.c.entityNum];//Defines the entity?
	//MOD1 ADDED END
	


	
	//MOD1 ADDED: Applying damage to entity
	//If we are allowed to attack
	gameLocal.Printf("Apply Damage!\n");
	if (ent) {//If the entity was defined
		gameLocal.Printf("Recognized entity");
		if (ent->fl.takedamage) {//If the entity can be damaged
			gameLocal.Printf("Entity taking damage");
			float dmgScale = 1.0f;
			ent->Damage(owner, owner, playerViewAxis[0], spawnArgs.GetString("def_damage"), dmgScale, 0);//Here spawnArgs seems to take from def file
		}
	}
	
	
}

/*
===============================================================================
	States
===============================================================================
*/

CLASS_STATES_DECLARATION(rvWeaponShotgun)
STATE("Idle", rvWeaponShotgun::State_Idle)
STATE("Fire", rvWeaponShotgun::State_Fire)
//STATE("Reload", rvWeaponShotgun::State_Reload)//MOD1 TAKEN: Not using bullets
END_CLASS_STATES

/*
================
rvWeaponShotgun::State_Idle
================
*/
stateResult_t rvWeaponShotgun::State_Idle(const stateParms_t& parms) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};
	switch (parms.stage) {
	case STAGE_INIT:
		//MOD1 TAKEN: not considering if weapon has bullets
		/*
		if (!AmmoAvailable()) {
			SetStatus(WP_OUTOFAMMO);
		}
		else {
			SetStatus(WP_READY);
		}
		*/

		SetStatus(WP_READY);																		//MOD1: readying the weapon?
		PlayCycle(ANIMCHANNEL_ALL, "idle", parms.blendFrames);
		return SRESULT_STAGE(STAGE_WAIT);

	case STAGE_WAIT:
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		if (wsfl.attack) {
			SetState("Fire", 0);
			//MOD1: Not included return statement. check if it works																
		}
		//MOD1 TAKEN: Set weapon to status Fire when there is no clipSize and other when there is clipsize
		/*
		if (!clipSize) {
			if (gameLocal.time > nextAttackTime && wsfl.attack && AmmoAvailable()) {
				SetState("Fire", 0);
				return SRESULT_DONE;
			}
		}
		else {
			if (gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip()) {
				SetState("Fire", 0);
				return SRESULT_DONE;
			}
			if (wsfl.attack && AutoReload() && !AmmoInClip() && AmmoAvailable()) {
				SetState("Reload", 4);
				return SRESULT_DONE;
			}
			if (wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable() > AmmoInClip())) {
				SetState("Reload", 4);
				return SRESULT_DONE;
			}
		}
		*/
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponShotgun::State_Fire
================
*/
stateResult_t rvWeaponShotgun::State_Fire(const stateParms_t& parms) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};
	//MOD1 ADDED START: Implementing damage calculation
	//Gauntlet uses entities and tracing to perform damage. Basically, defined entity and if it enters in contact then it applies damage to the entity
	

	switch (parms.stage) {
	case STAGE_INIT:
		gameLocal.Printf("Performing Attack!\n");
		//nextAttackTime was here
		//Attack(false, hitscans, spread, 0, 1.0f);//Will not be using hitscans
		nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier(PMOD_FIRERATE));
		Attack ( nextAttackTime );
		//MOD1 ADDED END
		PlayAnim(ANIMCHANNEL_ALL, "fire", 0);
		return SRESULT_STAGE(STAGE_WAIT);

	case STAGE_WAIT:
		if ((!gameLocal.isMultiplayer && (wsfl.lowerWeapon || AnimDone(ANIMCHANNEL_ALL, 0))) || AnimDone(ANIMCHANNEL_ALL, 0)) {
			SetState("Idle", 0);
			return SRESULT_DONE;
		}
		if (wsfl.attack && gameLocal.time >= nextAttackTime) { //MOD1 TAKEN: add "&& AmmoClip()" after nextAttackTi
			SetState("Fire", 0);
			return SRESULT_DONE;
		}
		//MOD1 TAKEN: We are not working with a clip
		/*
		if (clipSize) {
			if ((wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable() > AmmoInClip()))) {
				SetState("Reload", 4);
				return SRESULT_DONE;
			}
		}
		*/
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponShotgun::State_Reload
================

stateResult_t rvWeaponShotgun::State_Reload(const stateParms_t& parms) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
		STAGE_RELOADSTARTWAIT,
		STAGE_RELOADLOOP,
		STAGE_RELOADLOOPWAIT,
		STAGE_RELOADDONE,
		STAGE_RELOADDONEWAIT
	};
	switch (parms.stage) {
	case STAGE_INIT:
		if (wsfl.netReload) {
			wsfl.netReload = false;
		}
		else {
			NetReload();
		}

		SetStatus(WP_RELOAD);

		if (mods & SHOTGUN_MOD_AMMO) {
			PlayAnim(ANIMCHANNEL_ALL, "reload_clip", parms.blendFrames);
		}
		else {
			PlayAnim(ANIMCHANNEL_ALL, "reload_start", parms.blendFrames);
			return SRESULT_STAGE(STAGE_RELOADSTARTWAIT);
		}
		return SRESULT_STAGE(STAGE_WAIT);

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_ALL, 4)) {
			AddToClip(ClipSize());
			SetState("Idle", 4);
			return SRESULT_DONE;
		}
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;

	case STAGE_RELOADSTARTWAIT:
		if (AnimDone(ANIMCHANNEL_ALL, 0)) {
			return SRESULT_STAGE(STAGE_RELOADLOOP);
		}
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;

	case STAGE_RELOADLOOP:
		if ((wsfl.attack && AmmoInClip()) || AmmoAvailable() <= AmmoInClip() || AmmoInClip() == ClipSize()) {
			return SRESULT_STAGE(STAGE_RELOADDONE);
		}
		PlayAnim(ANIMCHANNEL_ALL, "reload_loop", 0);
		return SRESULT_STAGE(STAGE_RELOADLOOPWAIT);

	case STAGE_RELOADLOOPWAIT:
		if ((wsfl.attack && AmmoInClip()) || wsfl.netEndReload) {
			return SRESULT_STAGE(STAGE_RELOADDONE);
		}
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		if (AnimDone(ANIMCHANNEL_ALL, 0)) {
			AddToClip(1);
			return SRESULT_STAGE(STAGE_RELOADLOOP);
		}
		return SRESULT_WAIT;

	case STAGE_RELOADDONE:
		NetEndReload();
		PlayAnim(ANIMCHANNEL_ALL, "reload_end", 0);
		return SRESULT_STAGE(STAGE_RELOADDONEWAIT);

	case STAGE_RELOADDONEWAIT:
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		if (wsfl.attack && AmmoInClip() && gameLocal.time > nextAttackTime) {
			SetState("Fire", 0);
			return SRESULT_DONE;
		}
		if (AnimDone(ANIMCHANNEL_ALL, 4)) {
			SetState("Idle", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
*/

