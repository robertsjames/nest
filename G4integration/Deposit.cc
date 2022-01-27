#include "Deposit.hh"

std::shared_ptr<Deposits::Deposit> Deposits::Deposit::Instance()
{
    static std::shared_ptr<Deposits::Deposit> deposit{new Deposits::Deposit};
    return deposit;
}

Deposits::Deposit::Deposit()
{
}

void Deposits::Deposit::PlaceDeposit(G4Event* event, std::unique_ptr<const Deposits::EnergyDeposit> deposit)
{
    fDeposits.emplace_back(std::make_pair(event->GetEventID(), std::move(deposit)));
}
