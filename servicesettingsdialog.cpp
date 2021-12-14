#include "servicesettingsdialog.h"
#include "ui_servicesettingsdialog.h"

ServiceSettingsDialog::ServiceSettingsDialog(ServiceConfigurer *serviceConfigurer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServiceSettingsDialog) {
    ui->setupUi(this);
    this->serviceConfigurer = serviceConfigurer;
}

ServiceSettingsDialog::~ServiceSettingsDialog() {
    delete ui;
}

void ServiceSettingsDialog::setServiceParams(ServiceParams serviceParams) {
    this->serviceParams = serviceParams;
    ui->serviceFullNameEdit->setText(serviceParams.fullName);
    ui->serviceFilenameEdit->setText(serviceParams.fileName);
    ui->serviceLaunchPathEdit->setText(serviceParams.filePath);
    ui->serviceLaunchParamsEdit->setText(serviceParams.launchParams);
    ui->serviceLaunchCommandEdit->setText(serviceParams.launchCommand);
    ui->serviceParamsEdit->setText(serviceParams.serviceParams);
    ui->serviceLaunchDelayBox->setValue(serviceParams.launchDelay);
    ui->startedRegexEdit->setText(serviceParams.startedRegex);
    for (auto val: serviceConfigurer->getServiceTypes().keys()) {
        ui->serviceTypeBox->addItem(val);
    }
    int index = ui->serviceTypeBox->findText(serviceParams.type);
    if (index >= 0) {
        ui->serviceTypeBox->setCurrentIndex(index);
    }
    ui->resultLaunchCommandEdit->setText(serviceConfigurer->getLaunchCommand(serviceParams));
}

ServiceParams ServiceSettingsDialog::getServiceParams() {
    updateServiceParams();
    return serviceParams;
}

void ServiceSettingsDialog::paramChanged() {
    updateServiceParams();
    ui->resultLaunchCommandEdit->setText(serviceConfigurer->getLaunchCommand(serviceParams));
}

void ServiceSettingsDialog::updateServiceParams() {
    serviceParams.fullName = ui->serviceFullNameEdit->text();
    serviceParams.fileName = ui->serviceFilenameEdit->text();
    serviceParams.filePath = ui->serviceLaunchPathEdit->text();
    serviceParams.launchCommand = ui->serviceLaunchCommandEdit->text();
    serviceParams.launchParams = ui->serviceLaunchParamsEdit->toPlainText();
    serviceParams.serviceParams = ui->serviceParamsEdit->toPlainText();
    serviceParams.launchDelay = ui->serviceLaunchDelayBox->value();
    serviceParams.type = ui->serviceTypeBox->currentText();
    serviceParams.startedRegex = ui->startedRegexEdit->text();
}
