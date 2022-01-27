#include "Deposit.hh"

Deposits::Deposit::Deposit()
{
}

Deposits::Deposit::~Deposit()
{
}

void Deposits::Deposit::PlaceDeposit(G4Event* event, std::unique_ptr<const Deposits::EnergyDeposit> deposit)
{
    fDeposits.emplace_back(std::make_pair(event->GetEventID(), std::move(deposit)));
}
