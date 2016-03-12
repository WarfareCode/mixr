//------------------------------------------------------------------------------
// Classes:
//      SimLogger                   -- Simulation Event logger
//      SimLogger::SimLogEvent      -- Abstract simulation log event
//      SimLogger::NewPlayer        -- New Player event
//      SimLogger::LogPlayerData    -- Log Player Data event
//      SimLogger::RemovePlayer     -- Remove Player event
//      SimLogger::WeaponRelease    -- Weapon Release
//      SimLogger::GunFired         -- Gun was fired
//      SimLogger::KillEvent        -- Kill
//      SimLogger::NewTrack         -- Added new RADAR track
//      SimLogger::UpdateTrack      -- Updated RADAR track
//      SimLogger::RemovedTrack     -- Removed old RADAR track
//      SimLogger::NewRwrTrack      -- Added new RWR track
//      SimLogger::UpdateRwrTrack   -- Updated RWR track
//      SimLogger::RemovedRwrTrack  -- Removed old RWR track
//      SimLogger::DetonationEvent  -- Weapon Detonated
//------------------------------------------------------------------------------
#ifndef __oe_simulation_SimLogger_H__
#define __oe_simulation_SimLogger_H__

#include "openeaagles/base/Logger.h"
#include "openeaagles/base/safe_queue.h"
#include "openeaagles/base/osg/Vec3"

namespace oe {
   namespace base {
      class Identifier;
   }
namespace simulation {

class Emission;
class Player;
class Simulation;
class Track;
class TrackManager;

//------------------------------------------------------------------------------
// Class: SimLogger
// Base class:  base::Object -> base::Component -> base::Logger -> SimLogger
// Description: Simulation Event & Data Logger
//
// Factory name: SimLogger
// Slots:
//    timeline          <base::Identifier>  ! Source of the timeline { UTC, SIM or EXEC } (default: UTC)
//    includeUtcTime    <base::Number>      ! whether to record UTC time                  (default: true)
//    includeSimTime    <base::Number>      ! whether to record SIM time                  (default: true)
//    includeExecTime   <base::Number>      ! whether to record EXEC time                 (default: true)
//------------------------------------------------------------------------------
class SimLogger : public base::Logger
{
    DECLARE_SUBCLASS(SimLogger, base::Logger)

public:
    enum TSource {   // Source of the time line
       EXEC,         // R/T executive time
       SIM,          // Simulated time
       UTC           // Computer's UTC
    };

    class SimLogEvent;

public:
    SimLogger();

    void log(LogEvent* const event) override;               // Log a simulation event

    double getCurrentTime() const   { return time; }        // Current time (sec)
    TSource getTimeline() const     { return timeline; }    // Timeline (UTC, SIM or EXEC)

    void updateTC(const double dt = 0.0) override;
    void updateData(const double dt = 0.0) override;

protected:
   virtual bool setTimeline(const TSource ts);                             // Sets the logger's timeline (UTC, SIM or EXEC)
   virtual bool setIncludeUtcTime(const bool b);                           // whether to record UTC time
   virtual bool setIncludeSimTime(const bool b);                           // whether to record SIM time
   virtual bool setIncludeExecTime(const bool b);                          // whether to record EXEC time

   virtual bool setSlotTimeline(const base::Identifier* const p);         // Sets the source of the time (UTC, SIM or EXEC)
   virtual bool setSlotIncludeUtcTime(const base::Number* const num);     // whether to record UTC time
   virtual bool setSlotIncludeSimTime(const base::Number* const num);     // whether to record SIM time
   virtual bool setSlotIncludeExecTime(const base::Number* const num);    // whether to record EXEC time

private:
    static const int MAX_QUEUE_SIZE = 1000;     // Max size of the logger event queue
    base::safe_queue<SimLogEvent*> seQueue;    // Sim Event Queue

    double          time;                       // Sim time (seconds)
    double          execTime;                   // Executive time (seconds)
    double          simTime;                    // Sim time (seconds)
    double          utcTime;                    // UTC time (seconds)
    TSource         timeline;                   // Source of our timeline
    bool            includeUtcTime;             // whether to record UTC time
    bool            includeSimTime;             // whether to record SIM time
    bool            includeExecTime;            // whether to record EXEC time

public:
    //==============================================================================
    // ######### Simulation Log Event Classes #########
    //==============================================================================

    //------------------------------------------------------------------------------
    // Class: SimLogger::SimLogEvent
    // Base class:  base::Object -> base::Logger::LogEvent -> SimLogEvent
    // Description: Abstract class for all simulation log events
    //------------------------------------------------------------------------------
    class SimLogEvent : public base::Logger::LogEvent {
        DECLARE_SUBCLASS(SimLogEvent,base::Logger::LogEvent)
    public:
        SimLogEvent();
        virtual void captureData() =0;
        void setTime(const double t)               { time = t;          }
        void setExecTime(const double t)           { execTime = t;      }
        void setUtcTime(const double t)            { utcTime = t;       }
        void setSimTime(const double t)            { simTime = t;       }
        void setPrintExecTime(const bool b)        { printExecTime = b; }
        void setPrintUtcTime(const bool b)         { printUtcTime = b;  }
        void setPrintSimTime(const bool b)         { printSimTime = b;  }

    protected:
        std::ostream& makeTimeMsg(std::ostream& sout);
        std::ostream& makeExecTimeMsg(std::ostream& sout);
        std::ostream& makeUtcTimeMsg(std::ostream& sout);
        std::ostream& makeSimTimeMsg(std::ostream& sout);
        std::ostream& makeTabTimeMsg(std::ostream& sout);
        std::ostream& makeTabTimeHdr(std::ostream& sout);
        std::ostream& makePlayerIdMsg(std::ostream& sout, const Player* const player);
        std::ostream& makePlayerDataMsg(std::ostream& sout, osg::Vec3 pos0, osg::Vec3 vel0, osg::Vec3 angles0);
        std::ostream& makeTrackDataMsg(std::ostream& sout, const Track* const trk);
        std::ostream& makeEmissionDataMsg(std::ostream& sout, const Emission* const em);
        double time;
        double execTime;                            // Executive time (seconds)
        double simTime;                             // Sim time (seconds)
        double utcTime;                             // UTC time (seconds)
        bool   printUtcTime;                        // whether to record UTC time
        bool   printSimTime;                        // whether to record SIM time
        bool   printExecTime;                       // whether to record EXEC time
        char* msg;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::NewPlayer
    // Base class:  base::Object -> base::Logger::LogEvent -> NewPlayer
    // Description: Log a new player event
    //------------------------------------------------------------------------------
    class NewPlayer : public SimLogEvent {
        DECLARE_SUBCLASS(NewPlayer,SimLogEvent)
    public:
        NewPlayer(Player* const p);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const Player> thePlayer;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::LogPlayerData
    // Base class:  base::Object -> base::Logger::LogEvent -> LogPlayerData
    // Description: Log player's data event
    //------------------------------------------------------------------------------
    class LogPlayerData : public SimLogEvent {
        DECLARE_SUBCLASS(LogPlayerData,SimLogEvent)
    public:
        LogPlayerData(Player* const p);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const Player> thePlayer;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
        double    alpha;
        double    beta;
        double    ias;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::RemovePlayer
    // Base class:  base::Object -> base::Logger::LogEvent -> RemovePlayer
    // Description: Log a remove player event
    //------------------------------------------------------------------------------
    class RemovePlayer : public SimLogEvent {
        DECLARE_SUBCLASS(RemovePlayer,SimLogEvent)
    public:
        RemovePlayer(Player* const p);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const Player> thePlayer;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::WeaponRelease
    // Base class:  base::Object -> base::Logger::LogEvent -> WeaponRelease
    // Description: Log a weapon release (e.g., missile launch) event
    //   (i.e., the 'player' released weapon, 'wpn' at the target player, 'tgt'.
    //------------------------------------------------------------------------------
    class WeaponRelease : public SimLogEvent {
        DECLARE_SUBCLASS(WeaponRelease,SimLogEvent)
    public:
        WeaponRelease(Player* const player, Player* const wpn, Player* const tgt);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Player> theWeapon;
        base::safe_ptr<const Player> theTarget;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::GunFired
    // Base class:  base::Object -> base::Logger::LogEvent -> GunFired
    // Description: Log a gun fire event; Player just fired a burst of 'n' rounds
    //------------------------------------------------------------------------------
    class GunFired : public SimLogEvent {
        DECLARE_SUBCLASS(GunFired,SimLogEvent)
    public:
        GunFired(Player* const player, const int n);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const Player> thePlayer;
        int rounds;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::KillEvent
    // Base class:  base::Object -> base::Logger::LogEvent -> KillEvent
    // Description: Log a kill event
    //    (i.e., Target, 'tgt', killed by weapon, 'wpn', released from the 'player'.
    //------------------------------------------------------------------------------
    class KillEvent : public SimLogEvent {
        DECLARE_SUBCLASS(KillEvent,SimLogEvent)
    public:
        KillEvent(Player* const player, Player* const wpn, Player* const tgt);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Player> theWeapon;
        base::safe_ptr<const Player> theTarget;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::DetonationEvent
    // Base class:  base::Object -> base::Logger::LogEvent -> DetonationEvent
    // Description: Log a detonation event
    //    (i.e., Target, 'tgt', killed by weapon, 'wpn', released from the 'player'.
    //------------------------------------------------------------------------------
    class DetonationEvent : public SimLogEvent {
        DECLARE_SUBCLASS(DetonationEvent,SimLogEvent)
    public:
        DetonationEvent(Player* const player, Player* const wpn, Player* const tgt, const unsigned int detType, const double distance = -1.0f);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Player> theWeapon;
        base::safe_ptr<const Player> theTarget;
        unsigned int detType;
        double missDist;
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::NewTrack
    // Base class:  base::Object -> base::Logger::LogEvent -> NewTrack
    // Description: Log an added track event
    //------------------------------------------------------------------------------
    class NewTrack : public SimLogEvent {
        DECLARE_SUBCLASS(NewTrack,SimLogEvent)
    public:
        NewTrack(TrackManager* const mgr, Track* const trk);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const TrackManager> theManager;
        base::safe_ptr<const Track> theTrack;
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Emission> theEmission;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
        osg::Vec3 tgtPos;
        osg::Vec3 tgtVel;
        osg::Vec3 tgtAngles;
        double sn;              // Signal/Noise
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::UpdateTrack
    // Base class:  base::Object -> base::Logger::LogEvent -> UpdateTrack
    // Description: Log a update track event
    //------------------------------------------------------------------------------
    class UpdateTrack : public SimLogEvent {
        DECLARE_SUBCLASS(UpdateTrack,SimLogEvent)
    public:
        UpdateTrack(TrackManager* const mgr, Track* const trk);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const TrackManager> theManager;
        base::safe_ptr<const Track> theTrack;
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Emission> theEmission;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
        osg::Vec3 tgtPos;
        osg::Vec3 tgtVel;
        osg::Vec3 tgtAngles;
        double sn;              // Signal/Noise
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::RemovedTrack
    // Base class:  base::Object -> base::Logger::LogEvent -> RemovedTrack
    // Description: Log a removed track event
    //------------------------------------------------------------------------------
    class RemovedTrack : public SimLogEvent {
        DECLARE_SUBCLASS(RemovedTrack,SimLogEvent)
    public:
        RemovedTrack(TrackManager* const mgr, Track* const trk);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const TrackManager> theManager;
        base::safe_ptr<const Track> theTrack;
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Emission> theEmission;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
        osg::Vec3 tgtPos;
        osg::Vec3 tgtVel;
        osg::Vec3 tgtAngles;
        double sn;              // Signal/Noise
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::NewRwrTrack
    // Base class:  base::Object -> base::Logger::LogEvent -> NewRwrTrack
    // Description: Log an added RWR track event
    //------------------------------------------------------------------------------
    class NewRwrTrack : public SimLogEvent {
        DECLARE_SUBCLASS(NewRwrTrack,SimLogEvent)
    public:
        NewRwrTrack(TrackManager* const mgr, Track* const trk);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const TrackManager> theManager;
        base::safe_ptr<const Track> theTrack;
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Emission> theEmission;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
        osg::Vec3 tgtPos;
        osg::Vec3 tgtVel;
        osg::Vec3 tgtAngles;
        double sn;              // Signal/Noise
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::UpdateRwrTrack
    // Base class:  base::Object -> base::Logger::LogEvent -> UpdateRwrTrack
    // Description: Log update RWR track event
    //------------------------------------------------------------------------------
    class UpdateRwrTrack : public SimLogEvent {
        DECLARE_SUBCLASS(UpdateRwrTrack,SimLogEvent)
    public:
        UpdateRwrTrack(TrackManager* const mgr, Track* const trk);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const TrackManager> theManager;
        base::safe_ptr<const Track> theTrack;
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Emission> theEmission;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
        osg::Vec3 tgtPos;
        osg::Vec3 tgtVel;
        osg::Vec3 tgtAngles;
        double sn;              // Signal/Noise
    };

    //------------------------------------------------------------------------------
    // Class: SimLogger::RemovedRwrTrack
    // Base class:  base::Object -> base::Logger::LogEvent -> RemovedRwrTrack
    // Description: Log a removed RWR track event
    //------------------------------------------------------------------------------
    class RemovedRwrTrack : public SimLogEvent {
        DECLARE_SUBCLASS(RemovedRwrTrack,SimLogEvent)
    public:
        RemovedRwrTrack(TrackManager* const mgr, Track* const trk);
        const char* getDescription() override;
        void captureData() override;
    private:
        void initData();
        base::safe_ptr<const TrackManager> theManager;
        base::safe_ptr<const Track> theTrack;
        base::safe_ptr<const Player> thePlayer;
        base::safe_ptr<const Emission> theEmission;
        osg::Vec3 pos;
        osg::Vec3 vel;
        osg::Vec3 angles;
        osg::Vec3 tgtPos;
        osg::Vec3 tgtVel;
        osg::Vec3 tgtAngles;
        double sn;              // Signal/Noise
    };

};

} // End simulation namespace
} // End oe namespace

#endif
