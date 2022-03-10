#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

const int HYPERBLASTER_SPARM_BATTERY = 6;
const int HYPERBLASTER_SPIN_SPEED	 = 300;

class rvWeaponHyperblaster : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponHyperblaster );

	rvWeaponHyperblaster ( void );

	virtual void			Spawn				( void );
	void					Save				( idSaveGame *savefile ) const;
	void					Restore				( idRestoreGame *savefile );
	void					PreSave				( void );
	void					PostSave			( void );

protected:

	jointHandle_t			jointBatteryView;
	bool					spinning;

	void					SpinUp				( void );
	void					SpinDown			( void );
	float					range;
	void					Attack				( int );

private:

	stateResult_t		State_Idle		( const stateParms_t& parms );
	stateResult_t		State_Fire		( const stateParms_t& parms );
	//tateResult_t		State_Reload	( const stateParms_t& parms ); //MOD1 EDIT: Not using bullets
	
	CLASS_STATES_PROTOTYPE ( rvWeaponHyperblaster );
};

CLASS_DECLARATION( rvWeapon, rvWeaponHyperblaster )
END_CLASS

/*
================
rvWeaponHyperblaster::rvWeaponHyperblaster
================
*/
rvWeaponHyperblaster::rvWeaponHyperblaster ( void ) {
}

/*
================
rvWeaponHyperblaster::Spawn
================
*/
void rvWeaponHyperblaster::Spawn ( void ) {
	jointBatteryView = viewAnimator->GetJointHandle ( spawnArgs.GetString ( "joint_view_battery" ) );//?
	spinning		 = false;
	
	SetState ( "Raise", 0 );	
	range = spawnArgs.GetFloat("range", "32"); //MOD1 EDIT: range definition
}

/*
================
rvWeaponHyperblaster::Save
================
*/
void rvWeaponHyperblaster::Save ( idSaveGame *savefile ) const {
	savefile->WriteJoint ( jointBatteryView );
	savefile->WriteBool ( spinning );
	savefile->WriteFloat(range);
}

/*
================
rvWeaponHyperblaster::Restore
================
*/
void rvWeaponHyperblaster::Restore ( idRestoreGame *savefile ) {
	savefile->ReadJoint ( jointBatteryView );
	savefile->ReadBool ( spinning );
	savefile->ReadFloat(range);
}

/*
================
rvWeaponHyperBlaster::PreSave
================
*/
void rvWeaponHyperblaster::PreSave ( void ) {

	SetState ( "Idle", 4 );

	StopSound( SND_CHANNEL_WEAPON, false );
	StopSound( SND_CHANNEL_BODY, false );
	StopSound( SND_CHANNEL_ITEM, false );
	StopSound( SND_CHANNEL_ANY, false );
	
}

/*
================
rvWeaponHyperBlaster::PostSave
================
*/
void rvWeaponHyperblaster::PostSave ( void ) {
}

/*
================
rvWeaponHyperblaster::SpinUp
================
*/
void rvWeaponHyperblaster::SpinUp ( void ) {
	if ( spinning ) {
		return;
	}
	
	if ( jointBatteryView != INVALID_JOINT ) {
		viewAnimator->SetJointAngularVelocity ( jointBatteryView, idAngles(0,HYPERBLASTER_SPIN_SPEED,0), gameLocal.time, 50 );
	}

	StopSound ( SND_CHANNEL_BODY2, false );
	StartSound ( "snd_battery_spin", SND_CHANNEL_BODY2, 0, false, NULL );
	spinning = true;
}

/*
================
rvWeaponHyperblaster::SpinDown
================
*/
void rvWeaponHyperblaster::SpinDown	( void ) {
	if ( !spinning ) {
		return;
	}
	
	StopSound ( SND_CHANNEL_BODY2, false );
	StartSound ( "snd_battery_spindown", SND_CHANNEL_BODY2, 0, false, NULL );

	if ( jointBatteryView != INVALID_JOINT ) {
		viewAnimator->SetJointAngularVelocity ( jointBatteryView, idAngles(0,0,0), gameLocal.time, 500 );
	}

	spinning = false;
}

void rvWeaponHyperblaster::Attack(int nAT) {
	gameLocal.Printf("Inside Attack function\n");
	//MOD1 ADDED START
	trace_t		tr; //Trace involved in damage application
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

CLASS_STATES_DECLARATION ( rvWeaponHyperblaster )
	STATE ( "Idle",				rvWeaponHyperblaster::State_Idle)
	STATE ( "Fire",				rvWeaponHyperblaster::State_Fire )
	//STATE ( "Reload",			rvWeaponHyperblaster::State_Reload ) //MOD1 EDIT: Not reloading needed
END_CLASS_STATES

/*
================
rvWeaponHyperblaster::State_Idle
================
*/
stateResult_t rvWeaponHyperblaster::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			//MOD1 EDIT: Not reloading weapon or doing anything
			/*
			if ( !AmmoAvailable ( ) ) {
				SetStatus ( WP_OUTOFAMMO );
			} else {
				SetStatus ( WP_READY );
			}
			

			SpinDown ( ); //???

			if ( ClipSize() ) {
				viewModel->SetShaderParm ( HYPERBLASTER_SPARM_BATTERY, (float)AmmoInClip()/ClipSize() );
			} else {
				viewModel->SetShaderParm ( HYPERBLASTER_SPARM_BATTERY, 1.0f );		
			}

			*/
			SetStatus(WP_READY); //MOD1 EDIT: added weapon ready status
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:			
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			if (wsfl.attack) {
				SetState("Fire", 0);
			}

			//MOD1 EDIT: taken out criteria for fire and reloading
			/*
			if ( !clipSize ) {
				if ( gameLocal.time > nextAttackTime && wsfl.attack && AmmoAvailable ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				} 
			} else {
				if ( gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}  
				if ( wsfl.attack && AutoReload() && !AmmoInClip ( ) && AmmoAvailable () ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}
				if ( wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable()>AmmoInClip()) ) {
					SetState ( "Reload", 4 );
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
rvWeaponHyperblaster::State_Fire
================
*/
stateResult_t rvWeaponHyperblaster::State_Fire ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			//SpinUp ( ); //MOD1: Not considered change if breaks
			nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
			Attack ( nextAttackTime );//Mod1: Original Attack ( false, 1, spread, 0, 1.0f );
			//MOD1: taken out ammo related stuff
			/*
			if ( ClipSize() ) {
				viewModel->SetShaderParm ( HYPERBLASTER_SPARM_BATTERY, (float)AmmoInClip()/ClipSize() );
			} else {
				viewModel->SetShaderParm ( HYPERBLASTER_SPARM_BATTERY, 1.0f );		
			}
			*/
			PlayAnim ( ANIMCHANNEL_ALL, "fire", 0 );	
			return SRESULT_STAGE ( STAGE_WAIT );
	
		case STAGE_WAIT:		
			if ( wsfl.attack && gameLocal.time >= nextAttackTime && !wsfl.lowerWeapon ) { //MOD1 EDIT: Original wsfl.attack && gameLocal.time >= nextAttackTime && AmmoInClip() && !wsfl.lowerWeapon
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( (!wsfl.attack || wsfl.lowerWeapon) && AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {//MOD1 EDIT: Original !wsfl.attack || !AmmoInClip() || wsfl.lowerWeapon) && AnimDone ( ANIMCHANNEL_ALL, 0
				SetState ( "Idle", 0 );
				return SRESULT_DONE;
			}		
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponHyperblaster::State_Reload
================

stateResult_t rvWeaponHyperblaster::State_Reload ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.netReload ) {
				wsfl.netReload = false;
			} else {
				NetReload ( );
			}
			
			SpinDown ( );

			viewModel->SetShaderParm ( HYPERBLASTER_SPARM_BATTERY, 0 );
			
			SetStatus ( WP_RELOAD );
			PlayAnim ( ANIMCHANNEL_ALL, "reload", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
			
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				AddToClip ( ClipSize() );
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

*/
			
